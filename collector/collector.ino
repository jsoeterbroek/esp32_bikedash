/*
*/
#include <../common/common.h>
#include <math.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <SPI.h>
#include <Wire.h>
#include <U8g2lib.h>
#include <esp_now.h>
#include <WiFi.h>
#include <HardwareSerial.h>
#include <TinyGPSPlus.h>
#include <ESPmDNS.h>
#include <NetworkUdp.h>
#include <ArduinoOTA.h>

const char* ssid = "XXXXXXXX";
const char* password = "XXXXXXXXX";

//variabls for blinking an LED with Millis
const int led = 2; // ESP32 Pin to which onboard LED is connected
unsigned long previousMillis = 0;  // will store last time LED was updated
const long interval = 1000;  // interval at which to blink (milliseconds)
int ledState = LOW;  // ledState used to set the LED

// MAC Address of your dashboard receiver
// 1c:69:20:cd:4c:e8
uint8_t dashboard_broadcastAddress[] = {0x1c, 0x69, 0x20, 0xcd, 0x4c, 0xe8};

// MAC Address of your esphome receiver
// and/or ESPNow MQTT hub
// cc:ba:97:08:51:44
uint8_t esphome_broadcastAddress[] = {0xcc, 0xba, 0x97, 0x08, 0x51, 0x44};

// MAC Address of collector
// 40:4c:ca:5f:7d:54

// PINOUTS
// 
// LCD oled display
//  I2C
// VCC  ---> 3V3, 
// GND  ---> GND 
// SDA  ---> LP_I2C_SDA  ->  pin 6
// SCL  ---> LP_I2C_SCL  ->  pin 7
//
// Temp module (ASAIR AM2301A)
// VCC  ---> 5V
// GND  ---> GND
// (DATA) to a digital GPIO pin 4

// GPS module (neo-6m)
// GPS Tx --> GPIO5
// GPS Rx --> GPIO2
//
#define RXD2 5
#define TXD2 2
#define GPS_BAUD 9600

// start OLED display
#define OLED_SDA 6
#define OLED_SCL 7
#define time_delay 2000

U8G2_SH1106_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, 7, 6, U8X8_PIN_NONE);
// end OLED display

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

bool gps_status = false;
String MY_IP = "192.168.178.38";

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

void u8g2_prepare() {
  u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.setFontRefHeightExtendedText();
  u8g2.setDrawColor(1);
  u8g2.setFontPosTop();
  u8g2.setFontDirection(0);
}

// GPS
// The TinyGPSPlus object
TinyGPSPlus gps;
HardwareSerial gpsSerial(1); // use UART1

void setup() {

  pinMode(led, OUTPUT);

  // Init Serial Monitor
  Serial.begin(115200);
  gpsSerial.begin(GPS_BAUD, SERIAL_8N1, RXD2, TXD2);
  Serial.println("gps.Serial started at 9600 baud rate");

  // initialize OLED
  u8g2.begin();
  u8g2_prepare();

  // Set device as a Wi-Fi Station
  // for ota
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  // Port defaults to 3232
  ArduinoOTA.setPort(3232);

  // Hostname defaults to esp3232-[MAC]
  ArduinoOTA.setHostname("collector");

  // No authentication by default
  ArduinoOTA.setPassword("admin");

  ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";
      Serial.println("Start updating " + type);
    })
    .onEnd([]() {
      Serial.println("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });

  ArduinoOTA.begin();

  // check wifi status
  if (WiFi.localIP()) {
    Serial.println("Wifi OK");
    WIFI_OK = true;
  }

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
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
    Serial.println("Failed to add peer");
    return;
  }
  // register second peer  
  memcpy(peerInfo.peer_addr, esphome_broadcastAddress, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
  
  // setup temp sensor
  dht.begin();

  String software = "esp32_bike ";
  software += String('V') + version_major() + "." + version_minor() + "." + version_patch();
  // output OLED
  u8g2.setCursor(0, 0);
  u8g2.print("// BMW R1100GS");
  u8g2.setCursor(0, 10);
  u8g2.print(software);
  u8g2.setCursor(0, 20);
  u8g2.print("data collector");
  u8g2.setCursor(0, 30);
  u8g2.print("setup..");
  u8g2.sendBuffer();
  delay(10000);
  SETUP_OK = true;

}

void display_status_lcd() {
  // display status on LCD
  u8g2.clearBuffer();
  u8g2_prepare();
  u8g2.setFontMode(1);  /* activate transparent font mode */

  // display WIFI status
  u8g2.setCursor(0, 0);
  u8g2.setDrawColor(2);
  if (WIFI_OK) {
    u8g2.print("WIFI_OK");
  } else {
    u8g2.setDrawColor(1);
    u8g2.drawBox(0, 0, 45, 10);
    u8g2.setDrawColor(2);
    u8g2.print(" WIFI_NOK");
  }
  // display ESP send status
  u8g2.setCursor(0, 10);
  u8g2.setDrawColor(2);
  if (ESP_SEND_OK) {
    u8g2.print("ESP_SEND_OK");
  } else {
    u8g2.setDrawColor(1);
    u8g2.drawBox(0, 10, 80, 10);
    u8g2.setDrawColor(2);    
    u8g2.print(" ESP_SEND_NOK");
  }
  // display GPS status
  u8g2.setCursor(0, 20);
  u8g2.setDrawColor(2);
  if (GPS_OK) {
    u8g2.print("GPS_OK");
  } else {
    u8g2.setDrawColor(1);
    u8g2.drawBox(0, 20, 80, 10);
    u8g2.setDrawColor(2);
    u8g2.print(" GPS_NOK");
  }
  // display GSM status
  u8g2.setCursor(0, 30);
  u8g2.setDrawColor(2);
  if (GSM_OK) {
    u8g2.print("GSM_OK");
  } else {
    u8g2.setDrawColor(1);
    u8g2.drawBox(0, 30, 80, 10);
    u8g2.setDrawColor(2);
    u8g2.print(" GSM_NOK");
  }
  // display TEMPstatus
  u8g2.setCursor(0, 40);
  u8g2.setDrawColor(2);
  if (TEMP_OK) {
    u8g2.print("TEMP_OK ");
  } else {
    u8g2.setDrawColor(1);
    u8g2.drawBox(0, 40, 80, 10);
    u8g2.setDrawColor(2);    
    u8g2.print(" TEMP_NOK");
  }
  // display BATT status
  u8g2.setCursor(0, 50);
  u8g2.setDrawColor(2);
  if (BATT_OK) {
    u8g2.print("BATT_OK");
  } else {
    u8g2.setDrawColor(1);
    u8g2.drawBox(0, 50, 80, 10);
    u8g2.setDrawColor(2);    
    u8g2.print(" BATT_NOK");
  }
  u8g2.sendBuffer();
}
 
void loop() {
 
  ArduinoOTA.handle();

  while (gpsSerial.available() > 0) {
    if (gps.encode(gpsSerial.read())) {
      displayGPSInfo();
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

  Serial.println(F("*----------------------------------------*"));
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
  outgoingReadings.batt_v = 12.1;
  
  // Send message via ESP-NOW
  //esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &outgoingReadings, sizeof(outgoingReadings));
  esp_err_t result = esp_now_send(0, (uint8_t *) &outgoingReadings, sizeof(outgoingReadings));
   
  if (result == ESP_OK) {
    Serial.println("ESP_NOW Sent with success");
  } else {
     Serial.println("ESP_NOW Error sending the data");
  }

  display_status_lcd();
  delay(2000);
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
   } else {
     Serial.println(F("LOCATION INVALID"));
   }
   if (gps.time.isValid()) {
     outgoingReadings.gps_time_hour = gps.time.hour();
     outgoingReadings.gps_time_minute = gps.time.minute();
     //outgoingReadings.gps_time_second = gps.time.second();
   } else {
     Serial.println(F("TIME INVALID"));
   }
   if (gps.speed.isValid()) {
     outgoingReadings.gps_speed_kmph = gps.speed.kmph();
   } else {
     Serial.println(F("SPEED INVALID"));
     outgoingReadings.gps_speed_kmph = 0;
   }
   outgoingReadings.gps_altitude_meters = gps.altitude.meters();
   outgoingReadings.gps_satellites = gps.satellites.value();

   // TODO FIXME:
   outgoingReadings.gps_status = gps_status;
   outgoingReadings.gps_age = 5;
}
