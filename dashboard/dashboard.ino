/*
 Collector
  A = collector MAC: FC:B4:67:F6:90:E0
  B = dashboard MAC: D8:BC:38:FA:D4:A4
*/
#include <dashboard.h>
#include <esp_now.h>
#include <WiFi.h>

#include <SPI.h>

/*  Install the "TFT_eSPI" library by Bodmer to interface with the TFT Display - https://github.com/Bodmer/TFT_eSPI
    *** IMPORTANT: User_Setup.h available on the internet will probably NOT work with the examples available at Random Nerd Tutorials ***
    *** YOU MUST USE THE User_Setup.h FILE PROVIDED IN THE LINK BELOW IN ORDER TO USE THE EXAMPLES FROM RANDOM NERD TUTORIALS ***
    FULL INSTRUCTIONS AVAILABLE ON HOW CONFIGURE THE LIBRARY: https://RandomNerdTutorials.com/cyd/ or https://RandomNerdTutorials.com/esp32-tft/   */
#include <TFT_eSPI.h>

// Install the "XPT2046_Touchscreen" library by Paul Stoffregen to use the Touchscreen - https://github.com/PaulStoffregen/XPT2046_Touchscreen
// Note: this library doesn't require further configuration
#include <XPT2046_Touchscreen.h>

#define version "1.1"

// REPLACE WITH THE MAC Address of your receiver 
uint8_t broadcastAddress[] = {0xfc, 0xb4, 0x67, 0xf6, 0x90, 0xe0};

TFT_eSPI tft = TFT_eSPI();
// TFT_eSprite sprite = TFT_eSprite(&tft);

// Touchscreen pins
#define XPT2046_IRQ 36   // T_IRQ
#define XPT2046_MOSI 32  // T_DIN
#define XPT2046_MISO 39  // T_OUT
#define XPT2046_CLK 25   // T_CLK
#define XPT2046_CS 33    // T_CS

SPIClass touchscreenSPI = SPIClass(VSPI);
XPT2046_Touchscreen touchscreen(XPT2046_CS, XPT2046_IRQ);

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
#define FONT_SIZE 2

//Structure example to received data
//Must match the sender's structure
typedef struct struct_speedo {
  uint8_t speed_kph;
  uint8_t speed_rpm;
  int8_t fuel_perc;
  bool collector_is_dev;
} struct_speedo;

// Create a struct_message to hold incoming readings
struct_speedo collectorData;

esp_now_peer_info_t peerInfo;

// callback function that will be executed when data is received
void OnDataRecv(const esp_now_recv_info* mac, const uint8_t *incomingData, int len) {
  memcpy(&collectorData, incomingData, sizeof(collectorData));
}
 
void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  } else {
    Serial.println("Initializing ESP-NOW");
  }
  delay(50);
  
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(OnDataRecv);

  // Start the SPI for the touchscreen and init the touchscreen
  touchscreenSPI.begin(XPT2046_CLK, XPT2046_MISO, XPT2046_MOSI, XPT2046_CS);
  touchscreen.begin(touchscreenSPI);
  // Set the Touchscreen rotation in landscape mode
  touchscreen.setRotation(1);

  // Start the tft display
  tft.init();
  // Set the TFT display rotation in landscape mode
  tft.setRotation(1);

  // Clear the screen before writing to it
  tft.fillScreen(TFT_WHITE);
  tft.setTextColor(TFT_BLACK, TFT_WHITE);

  Serial.println( "Setup done" );
}
 

void loop()
{
  delay(100);
}
