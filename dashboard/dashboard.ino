/*******************************************************************************
 ******************************************************************************/
#include <SPI.h>
#include <esp_now.h>
#include <WiFi.h>
#include <TFT_eSPI.h>
#include "Free_Fonts.h" // Include the header file attached to this sketch

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

TFT_eSPI tft = TFT_eSPI();       // Invoke custom library
#define TEXT "aA MWyz~1298" // Text that will be printed on screen in any font

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
} struct_message;

// Create a struct_message to hold incoming data
struct_message myData;

//callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.print("temp: ");
  Serial.println(myData.temp);  
  Serial.print("hum: ");
  Serial.println(myData.hum);
}

void setup() {

  tft.init();
  tft.setRotation(1);
  
  tft.fillScreen(TFT_WHITE);
  // tft.fillCircle(120, 120, 110, TFT_BLACK);

  //Initialize Serial Monitor
  Serial.begin(115200);

  //Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  //Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));

  delay(2000); // 2 seconds
}

void loop() {

  draw();
  delay(1000);
}

void draw() {

  int32_t x = 0; int32_t y = 0; int32_t w = 0; int32_t h = 0;

  tft.fillScreen(TFT_GREY);            // Clear screen
  tft.setTextColor(TFT_WHITE);

  /*****************************************************************************
   * Temp 
   ****************************************************************************/
  char str_temp[50]; sprintf(str_temp, "%g", myData.temp);
  x = 4; y = 92; w = 82; h = 64;
  tft.drawRoundRect(x, y, w, h, 10, TFT_WHITE);
  tft.setFreeFont(FSSBO18);
  tft.drawString(str_temp, x+6, y+8, GFXFF);  
  tft.setFreeFont(FF1);
  tft.drawString("Temp", x+8, y+42, GFXFF);

  /*****************************************************************************
   * Humidity 
   ****************************************************************************/
  char str_hum[50]; sprintf(str_hum, "%g", myData.hum);
  x = 4; y = 160; w = 82; h = 64;
  tft.drawRoundRect(x, y, w, h, 10, TFT_WHITE);
  tft.setFreeFont(FSSBO18);
  tft.drawString(str_hum, x+6, y+8, GFXFF);  
  tft.setFreeFont(FF1);
  tft.drawString("Humid.", x+8, y+42, GFXFF);  

  delay(1000);
}
