/*
*/
#include <../common/common.h>
#include <DHT.h>
#include <SPI.h>
#include <Wire.h>
#include <esp_now.h>
#include <WiFi.h>
#include <HardwareSerial.h>
#include <TinyGPSPlus.h>
#include "BLEDevice.h"
#include "esp_log.h"
#include "mbedtls/aes.h"
#include <stdio.h>
#include <string.h>

// BLE for Battery Monitor
// B0:D2:78:29:5F:8B
//
// The remote service we wish to connect to.
static BLEUUID serviceUUID("0000fff0-0000-1000-8000-00805f9b34fb");

// The characteristic of the remote service we are interested in.
// battery voltage
static BLEUUID charUUID("0000fff4-0000-1000-8000-00805f9b34fb");

mbedtls_aes_context aes;
unsigned char key[16] = { 108, 101, 97, 103, 101, 110, 100, 255, 254, 49, 56, 56, 50, 52, 54, 54, };

static boolean doConnect = false;
static boolean connected = false;
static boolean doScan = false;
static BLERemoteCharacteristic *pRemoteCharacteristic;
static BLEAdvertisedDevice *myDevice;

// MAC Address of your dashboard receiver
// 1c:69:20:cd:4c:e8
uint8_t dashboard_broadcastAddress[] = {0x1c, 0x69, 0x20, 0xcd, 0x4c, 0xe8};

// PINOUTS
// 
//
// Temp module (ASAIR AM2301A)
// VCC  ---> 5V
// GND  ---> GND
// (DATA) to a digital GPIO pin 4

// GPS module (neo-6m)
// GPS Tx --> GPIO5
// GPS Rx --> GPIO2

// power converter
//  hw-613
// pin 1 vo+       power out (5v)
// pin 2 gnd	   GND 
// pin 3 in +      power in  (12v)  
// pin 4 en                   

// INA219
// VCC  ---> 5V, 
// GND  ---> GND 
// SDA  ---> LP_I2C_SDA  ->  pin 6
// SCL  ---> LP_I2C_SCL  ->  pin 7

#define RXD2 5
#define TXD2 2
#define GPS_BAUD 9600

// start Temp sensor
#define DHTPIN 4       // Digital pin connected to the DHT sensor
#define DHTTYPE DHT21  // DHT 21 (AM2301)
DHT dht(DHTPIN, DHTTYPE);
// Reading temperature or humidity takes about 250 milliseconds!
// Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
uint32_t delayMS;
// end Temp sensor

bool SETUP_OK = false;
bool ESP_SETUP_OK = false;
bool ESP_SEND_OK = false;
bool GPS_OK = false;
bool GPS_DATA_SEND_OK = false;
bool GPS_DEMO_DATA_SEND_ONCE = false;
bool GSM_OK = false;
bool TEMP_OK = false;
bool BATT_OK = false;
bool WIFI_OK = false;
bool BLE_CONNECT_OK = false;

bool gps_status = false;

// ESP Now
// Create a struct_message to hold outgoing readings
struct_message outgoingReadings;
esp_now_peer_info_t peerInfo;

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  char macStr[18];
  Serial.print("Packet to: ");
  // Copies the sender mac address to a string
  snprintf(macStr, sizeof(macStr), "%02x:%02x",
           mac_addr[0], mac_addr[1]);
  Serial.print(macStr);
  Serial.print(" send status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  if (status ==0){
    ESP_SEND_OK = true;
  }
}

// GPS
// The TinyGPSPlus object
TinyGPSPlus gps;
HardwareSerial gpsSerial(1); // use UART1

// BLE
static void notifyCallback(BLERemoteCharacteristic *pBLERemoteCharacteristic, uint8_t *pData, size_t length, bool isNotify) {
  //Serial.print(F("Notify callback for characteristic "));
  //Serial.print(pBLERemoteCharacteristic->getUUID().toString().c_str());
  //Serial.print(F(" of data length "));
  //Serial.println(length);
  //Serial.print(F("data: "));
  //Serial.write(pData, length);
  //Serial.println();

  //Create buffer for decrypt output
  size_t bufferSize = 16;
  unsigned char decryptedMessage[bufferSize];
  memset(decryptedMessage, 0, bufferSize);
  unsigned char output[16];
  unsigned char iv[16] = {};
  mbedtls_aes_init(&aes);
  mbedtls_aes_setkey_dec(&aes, key, 128);
  mbedtls_aes_crypt_cbc(&aes, MBEDTLS_AES_DECRYPT, 16, iv, pData, output);
  mbedtls_aes_free(&aes);
  outgoingReadings.batt_volt = ((output[2] | (output[1] << 8)) >> 4) / 100.0f;
  outgoingReadings.batt_perc = output[3];
}

class MyClientCallback : public BLEClientCallbacks {
  void onConnect(BLEClient *pclient) {}

  void onDisconnect(BLEClient *pclient) {
    connected = false;
    Serial.println(F("onDisconnect"));
  }
};

bool connectToServer() {
  Serial.print(F("Forming a connection to "));
  Serial.println(myDevice->getAddress().toString().c_str());

  BLEClient *pClient = BLEDevice::createClient();
  Serial.println(F(" - Created client"));

  pClient->setClientCallbacks(new MyClientCallback());

  // Connect to the remove BLE Server.
  pClient->connect(myDevice);  // if you pass BLEAdvertisedDevice instead of address, it will be recognized type of peer device address (public or private)
  Serial.println(F(" - Connected to device"));
  pClient->setMTU(517);  //set client to request maximum MTU from server (default is 23 otherwise)

  // Obtain a reference to the service we are after in the remote BLE server.
  BLERemoteService *pRemoteService = pClient->getService(serviceUUID);
  if (pRemoteService == nullptr) {
    Serial.print(F("Failed to find our service UUID: "));
    Serial.println(serviceUUID.toString().c_str());
    pClient->disconnect();
    return false;
  }
  Serial.println(F(" - Found our service"));

   // Obtain a reference to the characteristic in the service of the remote BLE server.
  pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);
  if (pRemoteCharacteristic == nullptr) {
    Serial.print(F("Failed to find our characteristic UUID: "));
    Serial.println(charUUID.toString().c_str());
    pClient->disconnect();
    return false;
  }
  Serial.println(F(" - Found our characteristic"));

  // Read the value of the characteristic.
  if (pRemoteCharacteristic->canRead()) {
    String value = pRemoteCharacteristic->readValue();
    Serial.print(F("The characteristic value was: "));
    Serial.println(value.c_str());
  }

  if (pRemoteCharacteristic->canNotify()) {
    pRemoteCharacteristic->registerForNotify(notifyCallback);
  }

  connected = true;
  return true;
}

/**
 * Scan for BLE servers and find the first one that advertises the service we are looking for.
 */
class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
  /**
   * Called for each advertising BLE server.
   */
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    //Serial.print(F("BLE Advertised Device found: "));
    //Serial.println(advertisedDevice.toString().c_str());

    // FIXME: BLE address into var
    if (strcmp(advertisedDevice.getAddress().toString().c_str(), "b0:d2:78:29:5f:8b") == 0) {
    // We have found a device, let us now see if it contains the service we are looking for.
    //if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(serviceUUID)) {
      Serial.println(F("BLE Advertised Device found: "));
      Serial.println(advertisedDevice.toString().c_str());

      Serial.println(F("Stop scan and connect: "));
      BLEDevice::getScan()->stop();
      myDevice = new BLEAdvertisedDevice(advertisedDevice);
      doConnect = true;
      //doScan = true;
      doScan = false;
      BLE_CONNECT_OK = true;

    }  // Found our server
  }  // onResult
};  // MyAdvertisedDeviceCallbacks

void setup() {

  delay(5000);

  // Init Serial Monitor
  Serial.begin(115200);

  Serial.println(F("Starting BLE Client ..."));
  BLEDevice::init("");

  // Retrieve a Scanner and set the callback we want to use to be informed when we
  // have detected a new device.  Specify that we want active scanning and start the
  // scan to run for 5 seconds.
  BLEScan *pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true);
  pBLEScan->start(5, false);

  gpsSerial.begin(GPS_BAUD, SERIAL_8N1, RXD2, TXD2);
  Serial.println(F("gps.Serial started at 9600 baud rate"));

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println(F("Error initializing ESP-NOW"));
    return;
  } else {
    ESP_SETUP_OK = true;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
  
  // register peer
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  // register first peer  
  memcpy(peerInfo.peer_addr, dashboard_broadcastAddress, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println(F("Failed to add peer"));
    return;
  }
  
  // setup temp sensor
  dht.begin();

  delay(10000);
  SETUP_OK = true;
}

void loop() {
 
  BATT_OK = true;
  if (BATT_OK) {
    sendBattInfo();
  }

  // If the flag "doConnect" is true then we have scanned for and found the desired
  // BLE Server with which we wish to connect.  Now we connect to it.  Once we are
  // connected we set the connected flag to be true.
  if (doConnect == true) {
    if (connectToServer()) {
      Serial.println(F("We are now connected to the BLE device."));
    } else {
      Serial.println(F("We have failed to connect to the device; there is nothing more we will do."));
    }
    doConnect = false;
  }

  // If we are connected to a peer BLE Server, update the characteristic each time we are reached
  // with the current time since boot.
  if (connected) {
    String newValue = "Time since boot: " + String(millis() / 1000);
    Serial.println("Setting new characteristic value to \"" + newValue + "\"");

    // Set the characteristic's value to be the array of bytes that is actually a string.
    pRemoteCharacteristic->writeValue(newValue.c_str(), newValue.length());
  } else if (doScan) {
    BLEDevice::getScan()->start(0);  // this is just example to start scan after disconnect, most likely there is better way to do it in arduino
  }

  while (gpsSerial.available() > 0) {
    if (gps.encode(gpsSerial.read())) {
      // displayGPSInfo();
      GPS_OK = true;
      sendGPSInfo();
      GPS_DATA_SEND_OK = true;
      gps_status = true;
    }
  }
  if (millis() > 5000 && gps.charsProcessed() < 10) {
    Serial.println(F("ERROR: No GPS data.."));
    GPS_OK = false;
    GPS_DATA_SEND_OK = false;
  }

  //Serial.println(F("*----------------------------------------*"));
  // Get temperature event and print its value.
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    TEMP_OK = false;
  } else {
    TEMP_OK = true;
  }
  outgoingReadings.hum = h;
  outgoingReadings.temp = t;

  // Set values to send
  outgoingReadings.fuel_perc = 0;
  
  // Send message via ESP-NOW
  //esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &outgoingReadings, sizeof(outgoingReadings));
  esp_err_t result = esp_now_send(0, (uint8_t *) &outgoingReadings, sizeof(outgoingReadings));
   
  if (result == ESP_OK) {
    Serial.println(F("ESP_NOW Sent with success"));
  } else {
     Serial.println(F("ESP_NOW Error sending the data"));
  }

  delay(5000);
}

void displayBattInfo() {
  Serial.println(F(""));
}

void sendBattInfo() {
  //outgoingReadings.batt_v = batt_v;
}

void displayGPSInfo() {
  Serial.print(F("Satellites: ")); 
  Serial.println(gps.satellites.value());//   Serial.print(F("Location: ")); 
  if (gps.location.isValid()) {
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(","));
    Serial.print(gps.location.lng(), 6);
  } else {
    Serial.println(F("Location data invalid"));
  }
  Serial.print(F("  Date/Time: "));
  if (gps.date.isValid()) {
    Serial.print(gps.date.month());
    Serial.print(F("/"));
    Serial.print(gps.date.day());
    Serial.print(F("/"));
    Serial.print(gps.date.year());
  } else {
    Serial.println(F("Datetime data invalid"));
  }
  Serial.print(F(" "));
  if (gps.time.isValid()) {
    if (gps.time.hour() < 10) Serial.print(F("0"));
      Serial.print(gps.time.hour());
      Serial.print(F(":"));
    if (gps.time.minute() < 10) Serial.print(F("0"));
      Serial.print(gps.time.minute());
      Serial.print(F(":"));
    if (gps.time.second() < 10) Serial.print(F("0"));
      Serial.print(gps.time.second());
      Serial.print(F("."));
    if (gps.time.centisecond() < 10) Serial.print(F("0"));
      Serial.print(gps.time.centisecond());
  } else {
     Serial.println(F("Time data invalid"));
  }
  Serial.println();
}

void sendGPSInfo() {
   if (gps.location.isValid()) {
     long scale=10000000UL; //10 milion. Why ? see below:
     /*
     The GPS Module is givin you raw LAT and LNG data. LAT and LNG are given in
     degrees and billionths, as two separate values.
     The raw data won't be given to you in a decimal format such as 50.123456,
     but rather as 50 degrees and 1234567895 billionths.
     You want to store LAT (or LGN) in one variable (and not two, the virst
     variable for degrees, the other one for billionts).
     If you save it as a float number, it will be preceise only up to 5 decimal
     places.
     So here comes the important trick - you will store all the degrees and
     billionth values in a long data type variable.
     Degrees will be multiplied with the variable scale(10 milion)
       - e.g. 30 degrees N * 10000000UL = 300000000  (9 digits)
     Billionths will be devided by 100UL. Why ? To get a 7 digits number, that
     will be added to the 9 digits degree value
     (You want to leave alone the first 2 digits of your degrees value.
     Billionths should begin from the third digit).
     This technique is called integer scaleing.
     */
     long lat; long lon;
     lat = gps.location.rawLat().deg * scale + gps.location.rawLat().billionths / 100UL;
     if(gps.location.rawLat().negative) lat=-lat;
     lon = gps.location.rawLng().deg * scale + gps.location.rawLng().billionths / 100UL;
     if(gps.location.rawLng().negative) lon=-lon;
     outgoingReadings.gps_lat = lat;
     outgoingReadings.gps_lng = lon;
   }
   if (gps.time.isValid()) {
     outgoingReadings.gps_time_hour = gps.time.hour();
     outgoingReadings.gps_time_minute = gps.time.minute();
     //outgoingReadings.gps_time_second = gps.time.second();
   }
   if (gps.speed.isValid()) {
     outgoingReadings.gps_speed_kmph = gps.speed.kmph();
   } else {
     outgoingReadings.gps_speed_kmph = 0;
   }
   outgoingReadings.gps_altitude_meters = gps.altitude.meters();
   outgoingReadings.gps_satellites = gps.satellites.value();
}
