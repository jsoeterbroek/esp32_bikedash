#include <../common/common.h>
//#include <Adafruit_Sensor.h>
//#include <DHT.h>
//#include <SPI.h>
//#include <Wire.h>
//#include <esp_now.h>
//#include <WiFi.h>
//#include <U8g2lib.h>
// #include <TFT_eSPI.h>
#include <SoftwareSerial.h>
#include <TinyGPSPlus.h>
// #include <Free_Fonts.h>
// #include <Orbitron_Medium_20.h>

#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 320
#define TFT_GREY 0x5AEB

// TFT_eSPI tft = TFT_eSPI();

bool SETUP_OK = false;
bool GPS_OK = false;
bool GPS_DATA_OK = false;
bool GSM_OK = false;
bool TEMP_OK = false;
bool BATT_OK = false;

// gps_status = false;

// // GPS
// static const int RXPin = 44, TXPin = 43;
// static const uint32_t GPSBaud = 4800;

// // The TinyGPSPlus object
// TinyGPSPlus gps;
// SoftwareSerial ss(RXPin, TXPin);

void setup() {

  // tft.init();
  //tft.setRotation(1);
  // tft.setRotation(0);
  // tft.fillScreen(TFT_WHITE);

  // Init Serial Monitor
  Serial.begin(115200);
  Serial.println("setup");
    // ss.begin(GPSBaud);

    // setup 'splash' screen
  // tft.fillScreen(TFT_GREY);            // Clear screen
  // tft.setTextColor(TFT_WHITE); 
  // tft.setFreeFont(&Orbitron_Medium_20);
  // tft.drawString("// BMW R1100GS", 6, 104, GFXFF);
  // String software = " esp32_bike ";
  // software += String('V') + version_major() + "." + version_minor() + "." + version_patch();
  // tft.drawString(software, 6, 190, GFXFF);
  delay(5000); // 5 seconds
  // tft.fillScreen(TFT_GREY);            // Clear screen
}

void loop() {
  Serial.println("loop");
// #ifdef GPS_INDOORS_TEST
//   // sample NMEA stream
//   const char *gpsStreamDemo =
//     "$GPRMC,045103.000,A,3014.1984,N,09749.2872,W,0.67,161.46,030913,,,A*7C\r\n"
//     "$GPGGA,045104.000,3014.1985,N,09749.2873,W,1,09,1.2,211.6,M,-22.5,M,,0000*62\r\n"
//     "$GPRMC,045200.000,A,3014.3820,N,09748.9514,W,36.88,65.02,030913,,,A*77\r\n"
//     "$GPGGA,045201.000,3014.3864,N,09748.9411,W,1,10,1.2,200.8,M,-22.5,M,,0000*6C\r\n"
//     "$GPRMC,045251.000,A,3014.4275,N,09749.0626,W,0.51,217.94,030913,,,A*7D\r\n"
//     "$GPGGA,045253.000,3014.4273,N,09749.0628,W,1,09,1.3,206.9,M,-22.5,M,,0000*6F\r\n";
     
//   while (*gpsStreamDemo) {
//     if (gps.encode(*gpsStreamDemo++)) {
//       GPS_OK = true;
//       Serial.println("++++++++++++++++ DEMO DATA ++++++");
//       displayGPSInfo();
//       Serial.println("++++++++++++++++ DEMO DATA ++++++");
//       sendGPSInfo();
//       GPS_DATA_SEND_OK = true;
//       GPS_DEMO_DATA_SEND_ONCE = true;
//     }
//   }
// #else
//   while (ss.available() > 0) {
//     if (gps.encode(ss.read())) {
//       GPS_OK = true;
//       // sendGPSInfo();
//       GPS_DATA_OK = true;
//     }
//   }
// #endif

//   if (millis() > 5000 && gps.charsProcessed() < 10) {
//     Serial.println(F("ERROR: No GPS data.."));
//     GPS_OK = false;
//     GPS_DATA_OK = false;
//   }
//   displayGPSInfo();
  delay(1000);
}

// void displayGPSInfo() {
//   Serial.print(F("Satellites: ")); 
//   Serial.println(gps.satellites.value());  Serial.print(F("Location: ")); 
//   if (gps.location.isValid()) {
//     Serial.print(gps.location.lat(), 6);
//     Serial.print(F(","));
//     Serial.print(gps.location.lng(), 6);
//   } else {
//     Serial.print(F("INVALID"));
//   }
//   Serial.print(F("  Date/Time: "));
//   if (gps.date.isValid()) {
//     Serial.print(gps.date.month());
//     Serial.print(F("/"));
//     Serial.print(gps.date.day());
//     Serial.print(F("/"));
//     Serial.print(gps.date.year());
//   } else {
//     Serial.print(F("INVALID"));
//   }
//   Serial.print(F(" "));
//   if (gps.time.isValid()) {
//     if (gps.time.hour() < 10) Serial.print(F("0"));
//     Serial.print(gps.time.hour());
//     Serial.print(F(":"));
//     if (gps.time.minute() < 10) Serial.print(F("0"));
//     Serial.print(gps.time.minute());
//     Serial.print(F(":"));
//     if (gps.time.second() < 10) Serial.print(F("0"));
//     Serial.print(gps.time.second());
//     Serial.print(F("."));
//     if (gps.time.centisecond() < 10) Serial.print(F("0"));
//     Serial.print(gps.time.centisecond());
//   } else {
//     Serial.print(F("INVALID"));
//   }
//   Serial.println();
// }
