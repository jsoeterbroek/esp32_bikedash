/*******************************************************************************
 ******************************************************************************/
#include <../common/common.h>
#include <SPI.h>
#include <esp_now.h>
#include <WiFi.h>
#include <TFT_eSPI.h>
#include "Free_Fonts.h" // Include the header file attached to this sketch
#include <NotoSansBold36.h>
#include <Orbitron_Medium_20.h>

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
}

void setup() {

  tft.init();
  //tft.setRotation(1);
  tft.setRotation(0);
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
  tft.setFreeFont(&Orbitron_Medium_20);
  tft.drawString("// BMW R1100GS", 6, 104, GFXFF);
  String software = " esp32_bike ";
  software += String('V') + version_major() + "." + version_minor() + "." + version_patch();
  tft.drawString(software, 6, 190, GFXFF);
  delay(5000); // 5 seconds
  tft.fillScreen(TFT_GREY);            // Clear screen
}

void loop() {

  if (myData.gps_status) {
    GPS_DATA_RECVD_OK = true;
  }
  if (ESP_DATA_RECVD_OK) {
    draw();
  }
  delay(5000);
}

void draw_no_esp() {

  tft.setTextColor(TFT_WHITE);
  tft.setFreeFont(FF1);

}

void draw() {

  tft.setTextColor(TFT_WHITE);
  tft.setFreeFont(FSS9);

  int32_t lx = 0; int32_t ly = 0; int32_t ls = 0;
  /*****************************************************************************
   * 'led' warning lights
   *
   *  esp -> green  : data received ok
   *  esp -> red   : data not received
   * 
   *  gps -> green  : outdoors, data received ok
   *  gps -> red   : data not received
   ****************************************************************************/
  lx = 20; ly = 5; ls = 4;
  tft.setFreeFont(FSS9);
  tft.drawString("esp", lx, ly, GFXFF);  
  if (ESP_DATA_RECVD_OK) {
    tft.fillSmoothCircle(lx-10, ly+9, ls, TFT_GREEN);
  } else {
    tft.fillSmoothCircle(lx-10, ly+9, ls, TFT_RED);
  }

  lx = 72; ly = 5; ls = 4;
  tft.setFreeFont(FSS9);
  tft.drawString("gps", lx, ly, GFXFF);  
  if (GPS_DATA_RECVD_OK) {
    tft.fillSmoothCircle(lx-10, ly+9, ls, TFT_GREEN);
  } else {
    tft.fillSmoothCircle(lx-10, ly+9, ls, TFT_RED);
  }

  tft.setTextColor(TFT_ORANGE);
  int32_t x = 0; int32_t y = 0; int32_t w = 0; int32_t h = 0;
  /*****************************************************************************
   * Batt 
   ****************************************************************************/
  x = 8; y = 30; w = 110; h = 64;
  tft.fillRoundRect(x, y, w, h, 10, TFT_WHITE);
  tft.setFreeFont(&Orbitron_Light_32);
  tft.setCursor(x+8, y+34);
  tft.println(myData.batt_v, 1);
  tft.setFreeFont(&Orbitron_Light_24);
  tft.setCursor(x+8, y+58);
  tft.println("batt");
  /*****************************************************************************
   * Temp 
   ****************************************************************************/
  x = 8; y = 96; w = 110; h = 64;
  tft.fillRoundRect(x, y, w, h, 10, TFT_WHITE);
  tft.setFreeFont(&Orbitron_Light_32);
  tft.setCursor(x+8, y+34);
  tft.println(myData.temp, 1);
  tft.setFreeFont(&Orbitron_Light_24);
  tft.setCursor(x+8, y+58);
  tft.println("temp");
  /*****************************************************************************
   * Humidity 
   ****************************************************************************/
  x = 8; y = 162; w = 110; h = 64;
  tft.fillRoundRect(x, y, w, h, 10, TFT_WHITE);
  tft.setFreeFont(&Orbitron_Light_32);
  tft.setCursor(x+8, y+34);
  tft.println(myData.hum, 1);
  tft.setFreeFont(&Orbitron_Light_24);
  tft.setCursor(x+8, y+58);
  tft.println("hum");
  /*****************************************************************************
   * Free
   ****************************************************************************/
  x = 8; y = 228; w = 110; h = 64;
  tft.fillRoundRect(x, y, w, h, 10, TFT_WHITE);
  /*****************************************************************************
   * GPS Time 
   ****************************************************************************/
  x = 122; y = 30; w = 110; h = 64;
  tft.fillRoundRect(x, y, w, h, 10, TFT_WHITE);
  tft.setFreeFont(&Orbitron_Light_32);
  if (GPS_DATA_RECVD_OK) {
    // make string "HH:MM"
    // TODO: 24-hours format
    char buffer[50];
    sprintf(buffer, "%.2i:%.2i",
      myData.gps_time_hour,
      myData.gps_time_minute);
    String gps_time(buffer);
  tft.setCursor(x+8, y+34);
  tft.println(gps_time);
  }
  tft.setFreeFont(&Orbitron_Light_24);
  tft.setCursor(x+8, y+58);
  tft.println("time");
  /*****************************************************************************
   * GPS locations
   ****************************************************************************/  
  x = 122; y = 96; w = 110; h = 64;
  tft.fillRoundRect(x, y, w, h, 10, TFT_WHITE);
  /*****************************************************************************
   * GPS speed 
   ****************************************************************************/  
  //  double gps_speed_kmph
  x = 122; y = 162; w = 110; h = 64;
  tft.fillRoundRect(x, y, w, h, 10, TFT_WHITE);
  if (GPS_DATA_RECVD_OK) {
    tft.setFreeFont(&Orbitron_Light_32);
    tft.setCursor(x+8, y+34);
    tft.println(myData.gps_speed_kmph);
  }
  tft.setFreeFont(&Orbitron_Light_24);
  tft.setCursor(x+8, y+58);
  tft.println("speed");
  /*****************************************************************************
   * Fuel
   ****************************************************************************/
  x = 122; y = 228; w = 110; h = 64;
  tft.fillRoundRect(x, y, w, h, 10, TFT_WHITE);
  tft.setFreeFont(&Orbitron_Light_32);
  tft.setCursor(x+8, y+34);
  tft.println(myData.fuel_perc);
  tft.setFreeFont(&Orbitron_Light_24);
  tft.setCursor(x+8, y+58);
  tft.println("fuel"); 

  // version
  tft.setTextColor(TFT_WHITE);
  tft.setFreeFont(FSSO9);
  String rev = " ";
  rev += String('v') + version_major() + "." + version_minor() + "." + version_patch();
  tft.drawString(rev, 162, 300, GFXFF);
}

// -------------------------------------------------------------------------
// List files in SPIFFS memory
// -------------------------------------------------------------------------
void listFiles(void) {
  Serial.println();
  Serial.println("SPIFFS files found:");

  listDir(SPIFFS, "/", true);
  Serial.println();
  delay(1000);
}

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
