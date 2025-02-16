/*******************************************************************************
 ******************************************************************************/
#include <../common/common.h>
#include <SPI.h>
#include <esp_now.h>
#include <WiFi.h>
#include <TFT_eSPI.h>
#include "Free_Fonts.h" // Include the header file attached to this sketch

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


//       An abbreviation of the font name. Look at the list below to see
//       the abbreviations used, for example:
//
//       tft.setFreeFont(FSSBO24)
//
//       Where the letters mean:
//       F = Free font
//       M = Mono
//      SS = Sans Serif (double S to distinguish is form serif fonts)
//       S = Serif
//       B = Bold
//       O = Oblique (letter O not zero)
//       I = Italic
//       # =  point size, either 9, 12, 18 or 24

#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 320
#define TFT_GREY 0x5AEB

TFT_eSPI tft = TFT_eSPI();

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

// ESPNow checks
volatile bool data_ready = false;
bool startup_ready = false;
bool startup_complete = false;

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
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
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

  // if (startup_ready && !startup_complete) {
  //     delay(120);
  //     start_splash();
  //     startup_ready = false;
  //  }

  if (ESP_DATA_RECVD_OK) {
    draw();
  }

  delay(5000);
}

void draw() {

  int32_t x = 0; int32_t y = 0; int32_t w = 0; int32_t h = 0;

  tft.fillScreen(TFT_WHITE);            // Clear screen
  tft.setTextColor(TFT_GREY);

  /*****************************************************************************
   * GPS Time 
   ****************************************************************************/
  x = 170; y = 4; w = 140; h = 64;
  tft.drawRoundRect(x, y, w, h, 10, TFT_GREY);
  tft.setFreeFont(FSSBO18);
  // make string "HH:MM"
  // TODO: 24-hours format
  int fu;
  char buffer[50];
  fu = sprintf(buffer, "%.2i:%.2i",
    myData.gps_time_hour,
    myData.gps_time_minute);
  String gps_time(buffer);
  tft.drawString(gps_time, x+6, y+8);
  tft.setFreeFont(FF1);
  tft.drawString("GPS Time", x+10, y+42, GFXFF);

  /*****************************************************************************
   * GPS locations
   ****************************************************************************/  
  x = 170; y = 70; w = 140; h = 70;
  tft.drawRoundRect(x, y, w, h, 10, TFT_GREY);
  tft.setFreeFont(FF1);
  tft.drawNumber(myData.gps_lat, x+6, y+8);  
  tft.drawNumber(myData.gps_lng, x+6, y+28);  
  tft.drawString("GPS lat/lon", x+6, y+50, GFXFF);

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
