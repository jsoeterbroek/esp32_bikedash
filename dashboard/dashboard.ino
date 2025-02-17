/*******************************************************************************
 ******************************************************************************/
#include <../common/common.h>
#include <SPI.h>
#include <esp_now.h>
#include <WiFi.h>
#include <TFT_eSPI.h>
#include "Free_Fonts.h" // Include the header file attached to this sketch
#include <NotoSansBold36.h>

#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 320
#define TFT_GREY 0x5AEB

TFT_eSPI tft = TFT_eSPI();

// status flags
bool SETUP_OK = false;
bool ESP_SETUP_OK = false;
bool ESP_DATA_RECVD_OK = false;
bool GPS_OK = false;
bool GPS_DATA_RECVD_OK = false;
bool GSM_OK = false;
bool GSM_DATA_RECVD_OK = false;
bool TEMP_OK = false;
bool TEMP_DATA_RECVD_OK = false;
bool BATT_OK = false;
bool BATT_DATA_RECVD_OK = false;

//Structure example to send data
//Must match the receiver structure
typedef struct struct_message {
    int id;
    float temp;
    float hum;
    unsigned int readingId;
    uint8_t speed_kph;
    uint8_t speed_rpm;
    int8_t fuel_perc;
    long gps_lat; // latitude - ex '30.239773'
    long gps_lng; // longitude - ex '-97.815685'
    uint8_t gps_time_hour;
    uint8_t gps_time_minute;
    uint8_t gps_time_second;
    double gps_speed_kmph; // current ground speed
    double gps_altitude_meters; // latest altitude fix
    int8_t gps_age; // mls since last update
} struct_message;

// Create a struct_message to hold incoming data
struct_message myData;

//callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.println("GPS data: ");
  Serial.print("  GPS time: ");
  Serial.println(myData.gps_time_hour);
  Serial.println(myData.gps_time_minute);
  Serial.println(myData.gps_time_second);
  Serial.print("  GPS lat: ");
  Serial.println(myData.gps_lat);
  Serial.print("  GPS long: ");
  Serial.println(myData.gps_lng);
  Serial.println("-------");
  ESP_DATA_RECVD_OK = true;
  // TODO: check actual GPS data
  GPS_DATA_RECVD_OK = true;
}

void setup() {

  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_WHITE);

  //Initialize Serial Monitor
  Serial.begin(115200);

  //Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  //Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  ESP_SETUP_OK = true;
  // Once ESPNow is successfully Init, we will register for recv CB
  // to get recv packer info
  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));

  // setup 'splash' screen
  tft.fillScreen(TFT_GREY);            // Clear screen
  tft.setTextColor(TFT_WHITE); 
  tft.setFreeFont(FSSBO18);
  tft.drawString("/// BMW R1100GS", 6, 104, GFXFF);
  tft.setFreeFont(FM9);
  String software = " esp32_bike ";
  software += String('V') + version_major() + "." + version_minor() + "." + version_patch();
  tft.drawString(software, 30, 144, GFXFF);

  delay(5000); // 5 seconds
}

void loop() {

  if (ESP_DATA_RECVD_OK) {
    draw();
  }

  delay(5000);
}

void draw() {

  int32_t x = 0; int32_t y = 0; int32_t w = 0; int32_t h = 0;

  tft.fillScreen(TFT_WHITE);            // Clear screen
  tft.setTextColor(TFT_GREY);
  tft.setFreeFont(FF1);

  /*****************************************************************************
   * 'led' warning lights
   * 
   *  gps -> orange : indoors, (test) data received ok
   *  gps -> green  : outdoors, data received ok
   *  gps -> grey   : data not received
   ****************************************************************************/
  tft.drawString("gps", 120, 20, GFXFF);  
  if (GPS_INDOORS_TEST) {
      if (GPS_DATA_RECVD_OK) {
          tft.fillSmoothCircle(110, 26, 4, TFT_ORANGE);
      } else {
          tft.fillSmoothCircle(110, 26, 4, TFT_GREY);
      }
  } else {
      if (GPS_DATA_RECVD_OK) {
          tft.fillSmoothCircle(110, 26, 4, TFT_GREEN);
      } else {
          tft.fillSmoothCircle(110, 26, 4, TFT_GREY);
      }
  }

  /*****************************************************************************
   * GPS Time 
   ****************************************************************************/
  x = 170; y = 4; w = 140; h = 64;
  tft.drawRoundRect(x, y, w, h, 10, TFT_GREY);
  tft.setFreeFont(FSSBO18);
  if (GPS_DATA_RECVD_OK) {
    // make string "HH:MM"
    // TODO: 24-hours format
    char buffer[50];
    sprintf(buffer, "%.2i:%.2i",
      myData.gps_time_hour,
      myData.gps_time_minute);
    String gps_time(buffer);
    tft.drawString(gps_time, x+6, y+8);
  }
  tft.setFreeFont(FF1);
  tft.drawString("GPS Time", x+10, y+42, GFXFF);

  /*****************************************************************************
   * GPS locations
   ****************************************************************************/  
  x = 170; y = 70; w = 140; h = 70;
  tft.drawRoundRect(x, y, w, h, 10, TFT_GREY);
  tft.setFreeFont(FF1);
  if (GPS_DATA_RECVD_OK) {
    tft.drawNumber(myData.gps_lat, x+6, y+8);  
    tft.drawNumber(myData.gps_lng, x+6, y+28);  
  }
  tft.drawString("GPS lat/lon", x+6, y+50, GFXFF);

  /*****************************************************************************
   * GPS speed 
   ****************************************************************************/  
  //  double gps_speed_kmph
  x = 170; y = 142; w = 140; h = 70;
  tft.drawRoundRect(x, y, w, h, 10, TFT_GREY);
  if (GPS_DATA_RECVD_OK) {
    //tft.setFreeFont(FSSBO18);
    tft.loadFont(NotoSansBold36);
    tft.drawNumber(myData.gps_speed_kmph, x+20, y+10);  
    tft.unloadFont();
  }
  tft.setFreeFont(FF1);
  tft.drawString("Ground Speed (K/h)", x+6, y+50, GFXFF);

  //  double gps_altitude_meters

  /*****************************************************************************
   * Temp 
   ****************************************************************************/
  x = 4; y = 102; w = 82; h = 64;
  tft.drawRoundRect(x, y, w, h, 10, TFT_GREY);
  tft.setFreeFont(FSSBO18);
  tft.drawFloat(myData.temp, 1, x+5, y+8);  
  tft.setFreeFont(FF1);
  tft.drawString("Temp", x+8, y+42, GFXFF);

  /*****************************************************************************
   * Humidity 
   ****************************************************************************/
  x = 4; y = 170; w = 82; h = 64;
  tft.drawRoundRect(x, y, w, h, 10, TFT_GREY);
  tft.setFreeFont(FSSBO18);
  tft.drawFloat(myData.hum, 0, x+5, y+8);  
  tft.setFreeFont(FF1);
  tft.drawString("Humid.", x+8, y+42, GFXFF);  

  delay(1000);
}

// -------------------------------------------------------------------------
// List files in ESP8266 or ESP32 SPIFFS memory
// -------------------------------------------------------------------------
void listFiles(void) {
  Serial.println();
  Serial.println("SPIFFS files found:");

#ifdef ESP32
  listDir(SPIFFS, "/", true);
#else
  fs::Dir dir = SPIFFS.openDir("/"); // Root directory
  String  line = "=====================================";

  Serial.println(line);
  Serial.println("  File name               Size");
  Serial.println(line);

  while (dir.next()) {
    String fileName = dir.fileName();
    Serial.print(fileName);
    int spaces = 25 - fileName.length(); // Tabulate nicely
    if (spaces < 0) spaces = 1;
    while (spaces--) Serial.print(" ");
    fs::File f = dir.openFile("r");
    Serial.print(f.size()); Serial.println(" bytes");
    yield();
  }

  Serial.println(line);
#endif
  Serial.println();
  delay(1000);
}

#ifdef ESP32
void listDir(fs::FS &fs, const char * dirname, uint8_t levels) {
  Serial.printf("Listing directory: %s\n", dirname);

  fs::File root = fs.open(dirname);
  if (!root) {
    Serial.println("Failed to open directory");
    return;
  }
  if (!root.isDirectory()) {
    Serial.println("Not a directory");
    return;
  }

  fs::File file = root.openNextFile();
  while (file) {

    if (file.isDirectory()) {
      Serial.print("DIR : ");
      String fileName = file.name();
      Serial.print(fileName);
      if (levels) {
        listDir(fs, file.name(), levels - 1);
      }
    } else {
      String fileName = file.name();
      Serial.print("  " + fileName);
      int spaces = 32 - fileName.length(); // Tabulate nicely
      if (spaces < 1) spaces = 1;
      while (spaces--) Serial.print(" ");
      String fileSize = (String) file.size();
      spaces = 8 - fileSize.length(); // Tabulate nicely
      if (spaces < 1) spaces = 1;
      while (spaces--) Serial.print(" ");
      Serial.println(fileSize + " bytes");
    }

    file = root.openNextFile();
  }
}
#endif
