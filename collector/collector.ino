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
#include <SoftwareSerial.h>
#include <TinyGPSPlus.h>

// REPLACE WITH THE MAC Address of your receiver (dashboard)
// ESP-2432S032 1c:69:20:cd:4c:e8
uint8_t broadcastAddress[] = {0x1c, 0x69, 0x20, 0xcd, 0x4c, 0xe8};

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
//
// GPS module (neo-6m)
// VCC  ---> 5V
// GND  ---> GND
// RX   ---> software serial RX -> pin 1
// TX   ---> software serial TX -> pin 0
// pins 4(rx) and 3(tx).


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

gps_status = false;

// Variable to store if sending data was successful
String success;

// Create a struct_message to hold outgoing readings
struct_message outgoingReadings;

esp_now_peer_info_t peerInfo;

// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  if (status ==0){
    success = "Delivery Success :)";
    ESP_SEND_OK = true;
  } else{
    success = "Delivery Fail :(";
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
static const int RXPin = 1, TXPin = 0;
static const uint32_t GPSBaud = 4800;

// The TinyGPSPlus object
TinyGPSPlus gps;
SoftwareSerial ss(RXPin, TXPin);

void setup() {
  // Init Serial Monitor
  Serial.begin(115200);

  // initialize OLED
  u8g2.begin();
  u8g2_prepare();

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

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
  
  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
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

  ss.begin(GPSBaud);

}

void display_status_lcd() {
  // display status on LCD
  u8g2.clearBuffer();
  u8g2_prepare();
  u8g2.setFontMode(1);  /* activate transparent font mode */

  // display ESP setup status
  u8g2.setCursor(0, 0);
  u8g2.setDrawColor(2);
  if (ESP_SETUP_OK) {
    u8g2.print("ESP_SETUP_OK");
  } else {
    u8g2.setDrawColor(1);
    u8g2.drawBox(0, 0, 45, 10);
    u8g2.setDrawColor(2);
    u8g2.print(" ESP_SETUP_NOK");
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
 
#ifdef GPS_INDOORS_TEST
  // sample NMEA stream
  const char *gpsStreamDemo =
    "$GPRMC,045103.000,A,3014.1984,N,09749.2872,W,0.67,161.46,030913,,,A*7C\r\n"
    "$GPGGA,045104.000,3014.1985,N,09749.2873,W,1,09,1.2,211.6,M,-22.5,M,,0000*62\r\n"
    "$GPRMC,045200.000,A,3014.3820,N,09748.9514,W,36.88,65.02,030913,,,A*77\r\n"
    "$GPGGA,045201.000,3014.3864,N,09748.9411,W,1,10,1.2,200.8,M,-22.5,M,,0000*6C\r\n"
    "$GPRMC,045251.000,A,3014.4275,N,09749.0626,W,0.51,217.94,030913,,,A*7D\r\n"
    "$GPGGA,045253.000,3014.4273,N,09749.0628,W,1,09,1.3,206.9,M,-22.5,M,,0000*6F\r\n";
 
  while (*gpsStreamDemo) {
    if (gps.encode(*gpsStreamDemo++)) {
      GPS_OK = true;
      Serial.println("++++++++++++++++ DEMO DATA ++++++");
      displayGPSInfo();
      Serial.println("++++++++++++++++ DEMO DATA ++++++");
      sendGPSInfo();
      GPS_DATA_SEND_OK = true;
      GPS_DEMO_DATA_SEND_ONCE = true;
    }
  }
#else
  while (ss.available() > 0) {
    if (gps.encode(ss.read())) {
      displayGPSStatus();
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
#endif

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
  outgoingReadings.fuel_perc = 50;
  outgoingReadings.batt_v = 12.1;
  
  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &outgoingReadings, sizeof(outgoingReadings));
   
  if (result == ESP_OK) {
    Serial.println("ESP_NOW Sent with success");
  } else {
     Serial.println("ESP_NOW Error sending the data");
  }

  display_status_lcd();
  delay(8000);
}

void displayGPSStatus() {
  Serial.print(F("Satellites: ")); 
  Serial.println(gps.satellites.value());
}

void displayGPSInfo() {
  Serial.print(F("Location: ")); 
  if (gps.location.isValid()) {
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(","));
    Serial.print(gps.location.lng(), 6);
  } else {
    Serial.print(F("INVALID"));
  }
  Serial.print(F("  Date/Time: "));
  if (gps.date.isValid()) {
    Serial.print(gps.date.month());
    Serial.print(F("/"));
    Serial.print(gps.date.day());
    Serial.print(F("/"));
    Serial.print(gps.date.year());
  } else {
    Serial.print(F("INVALID"));
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
    Serial.print(F("INVALID"));
  }
  Serial.println();
}

void sendGPSInfo() {
  if (gps.location.isValid()) {
    long scale=10000000UL; //10 milion. Why ? This technique is called - integer scaling.
    /*
    The GPS Module is givin you raw LAT and LNG data. LAT and LNG are given in degrees and 
    billionths, as two separate values.
    The raw data won't be given to you in a decimal format such as 50.123456, but reather 
    as 50 degrees and 1234567895 billionths.
    You want to store LAT (or LGN) in one variable (and not two, the virst variable for degrees, 
    the other one for billionts).
    If you save it as a float number, it will be preceise only up to 5 decimal places.
    So here comes the important trick - you will store all the degrees and billionth values in 
    a long data type variable.
    Degrees will be multiplied with the variable scale(10 milion) 
    - e.g. 30 degrees N * 10000000UL = 300000000  (9 digits)
    Billionths will be devided by 100UL. Why ? To get a 7 digits number, that will be added to 
    the 9 digits degree value
    (You want to leave alone the first 2 digits of your degrees value. Billionths should begin 
    from the third digit)
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
    Serial.print(F("INVALID"));
  }
  if (gps.time.isValid()) {
    // Serial.print("DEBUG: GPS time value: ");
    // Serial.println(gps.time.value());   
    outgoingReadings.gps_time_hour = gps.time.hour();
    outgoingReadings.gps_time_minute = gps.time.value();
    outgoingReadings.gps_time_second = gps.time.value();
  } else {
    Serial.print(F("INVALID"));
  }
  outgoingReadings.gps_status = gps_status;
  // TODO: dummy data
  outgoingReadings.gps_speed_kmph = 122;
  outgoingReadings.gps_altitude_meters = 12;
  outgoingReadings.gps_age = 5;
}
