/*******************************************************************************
 ******************************************************************************/
#include <../common/common.h>
#include <esp_now.h>
#include <WiFi.h>
#include <TFT_eSPI.h>
#include <fonts/Free_Fonts.h>
#include <fonts/Orbitron_Medium_20.h>
#include <fonts/Latin_Hiragana_24.h>
#include <fonts/DSEG7.h>
#include <Pangodream_18650_CL.h>
#include <TJpg_Decoder.h>

// TODO: 18650 battery charge indicator
// https://github.com/0015/ThatProject/tree/master/ESP32_TTGO/TTGO_Battery_Indicator

#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 320
#define TFT_GREY      0x5AEB   // grey
#define FG_COLOR      0x9986   // red
#define RECT_FG_COLOR 0x5AEB   // grey
#define RECT_BG_COLOR 0xE73C   // white
#define BG_COLOR      0x5AEB   // grey

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite spr = TFT_eSprite(&tft);

// 18650
#define ICON_WIDTH 70
#define ICON_HEIGHT 36
#define STATUS_HEIGHT_BAR ICON_HEIGHT
#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))
#define ICON_POS_X (tft.width() - ICON_WIDTH)

//#define MIN_USB_VOL 4.9
//#define ADC_PIN 26
//#define CONV_FACTOR 1.8
//#define READS 20

//Pangodream_18650_CL BL(ADC_PIN, CONV_FACTOR, READS);
//Pangodream_18650_CL BL;
//char *batteryImages[] = {"/battery_01.jpg", "/battery_02.jpg", "/battery_03.jpg", "/battery_04.jpg", "/battery_05.jpg"};


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
  //Serial.println("GPS data: ");
  //Serial.println("  time: ");
  //Serial.print("    hour: "); Serial.println(myData.gps_time_hour);
  //Serial.print("    minute: "); Serial.println(myData.gps_time_minute);
  ////Serial.println("  satellites: ");
  //Serial.print("    found: "); Serial.println(myData.gps_satellites);
  //Serial.println("  location: ");
  //Serial.print("    lat: "); Serial.println(myData.gps_lat);  
  ////Serial.print("    lon: "); Serial.println(myData.gps_lng);  
  //Serial.println("  altitude: ");
  //Serial.print("    meters: "); Serial.println(myData.gps_altitude_meters);  
  //Serial.println("-------");
  ESP_DATA_RECVD_OK = true;
}

void setup() {

  //Initialize Serial Monitor
  Serial.begin(115200);

  //pinoutInit();

  tft.init();
  tft.setRotation(0);
  tft.fillScreen(BG_COLOR);

  //xTaskCreate(battery_info, "battery_info", 2048, NULL, 1, NULL);

  //Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  //Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  Serial.println("initialised ESP-NOW OK");
  ESP_SETUP_OK = true;

  // Once ESPNow is successfully Init, we will register
  // for recv CB to get recv packer info
  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));

  // setup 'splash' screen
  tft.setTextColor(TFT_WHITE); 
  tft.setFreeFont(&Orbitron_Medium_20);
  tft.drawString("// BMW R1100GS", 6, 104, GFXFF);
  String software = " esp32_bike ";
  software += String('V') + version_major() + "." + version_minor() + "." + version_patch();
  tft.drawString(software, 6, 190, GFXFF);
  delay(6000); // 6 seconds
  tft.fillScreen(BG_COLOR);            // Clear screen
}

void draw_display_box(int32_t _x, int32_t _y, float _display, uint8_t _d, String _display_unit, String _display_label) {
  int32_t _w = 115;
  int32_t _h = 70;
  spr.createSprite(_w, _h);
  spr.fillSprite(TFT_TRANSPARENT);
  // background 
  spr.setTextColor(RECT_BG_COLOR);
  spr.drawRoundRect(0, 0, _w, _h, 10, RECT_BG_COLOR);
  spr.fillRoundRect(1, 1, _w-2, _h-2, 10, RECT_FG_COLOR);
  // label
  spr.setTextDatum(TC_DATUM);
  spr.setFreeFont(FSSO9);
  spr.drawString("  ", 14, 6); // FIXME: small icon here; placeholder
  spr.drawString(_display_label, 54, 6);
  spr.drawString(_display_unit, 102, 6);
  // main display number
  spr.setTextDatum(MC_DATUM);
  spr.setFreeFont(&DSEG7_Classic_Bold_32);
  spr.drawFloat(_display, _d, _w / 2, 46);
  // push and delete sprite
  spr.pushSprite(_x, _y, TFT_TRANSPARENT);
  spr.deleteSprite();
}

void draw_no_esp() {

  // TODO: once design of main screen is settled, build this screen
  tft.setTextColor(FG_COLOR);
  tft.setFreeFont(FSS9);
  tft.setCursor(130, 18);
  tft.print("No ESP data received from collector..");
}

void draw() {

  // TODO: night and day display,
  // during day light background, dark foreground
  // during night dark background, light foreground
  tft.setTextColor(TFT_WHITE);
  tft.setFreeFont(FSS9);
  tft.fillRect(0, 0, 280, 24, TFT_ORANGE);

  int16_t lx = 0; int16_t ly = 0; int16_t ls = 0;
  /*****************************************************************************
   * 'led' warning lights
   *
   *  esp -> green  : data received ok
   *  esp -> red   : data not received
   * 
   *  gps -> green  : outdoors, data received ok
   *  gps -> red   : data not received
   ****************************************************************************/
  lx = 20; ly = 3; ls = 4;
  tft.setFreeFont(FSS9);
  tft.drawString("esp", lx, ly, GFXFF);  
  if (ESP_DATA_RECVD_OK) {
    tft.fillSmoothCircle(lx-10, ly+9, ls, TFT_GREEN);
  } else {
    tft.fillSmoothCircle(lx-10, ly+9, ls, TFT_RED);
  }

  lx = 72; ly = 3; ls = 4;
  tft.setFreeFont(FSS9);
  tft.drawString("gps", lx, ly, GFXFF);  
  if (GPS_DATA_RECVD_OK) {
    tft.fillSmoothCircle(lx-10, ly+9, ls, TFT_GREEN);
  } else {
    tft.fillSmoothCircle(lx-10, ly+9, ls, TFT_RED);
  }

  /*****************************************************************************
   * GPS satellites
   ****************************************************************************/
  tft.setFreeFont(FSS9);
  if (GPS_DATA_RECVD_OK) {
    tft.setCursor(130, 18);
    tft.print(myData.gps_satellites);
    //tft.println(" satt");
  }

 /*****************************************************************************
   * GPS Time 
   ****************************************************************************/
  lx = 186; ly = 18;
  if (GPS_DATA_RECVD_OK) {
    // make string "HH:MM"
    // TODO: 24-hours format, summertime & wintertime, CET timezone
    //   - 24-hours format
    //   - zomertijd/wintertijd +1 (timezone)
    //     for now hour + 1
    char buffer[50];
    sprintf(buffer, "%.2i:%.2i",
      myData.gps_time_hour+1,
      myData.gps_time_minute);
    String gps_time(buffer);
    tft.setCursor(lx, ly);
    tft.println(gps_time);  
  }

  tft.setTextColor(FG_COLOR);
  int32_t x = 0; int32_t y = 0; int32_t r = 10;
  int32_t t = 0; int32_t w = 0; int32_t h = 0;
  /*****************************************************************************
   * Battery status
   ****************************************************************************/
  // TODO: set battery status here
  lx = 186; ly = 18;

  /*****************************************************************************
   * Batt 
   ****************************************************************************/
  draw_display_box(4, 28, myData.batt_volt, 1, "V", "Batt");
  draw_display_box(122, 28, myData.batt_perc, 0, "%", "Batt");
  /*****************************************************************************
   * Temp & humidity 
   ****************************************************************************/
  draw_display_box(4, 100, myData.temp, 1, "C", "Temp");
  draw_display_box(122, 100, myData.hum, 1, "%", "Hum");
  /*****************************************************************************
   * Fuel
   ****************************************************************************/  
  draw_display_box(122, 172, myData.fuel_perc, 0, "%", "Fuel");
  /*****************************************************************************
   * GPS Altitude & ground speed
   ****************************************************************************/
  draw_display_box(4, 244, myData.gps_altitude_meters, 1, "m", "Alt");
  draw_display_box(122, 244, myData.gps_speed_kmph, 0, "Km", "Speed");
}

// 18650
//void pinoutInit(){
//  pinMode(14, OUTPUT);
//  digitalWrite(14, HIGH);
//}


// void battery_info(void *arg) {
//   while (1) {
//     tft.setCursor (0, STATUS_HEIGHT_BAR);
//     tft.println("");
//     tft.print("Average value from pin: ");
//     tft.println(BL.pinRead());
//     tft.print("Volts: ");
//     tft.println(BL.getBatteryVolts());
//     tft.print("Charge level: ");
//     tft.println(BL.getBatteryChargeLevel());
//     
//     if(BL.getBatteryVolts() >= MIN_USB_VOL) {
//       for(int i=0; i< ARRAY_SIZE(batteryImages); i++) {
//         drawingBatteryIcon(batteryImages[i]);
//         drawingText("Chrg");
//         vTaskDelay(500);
//       }
//     } else {
//         int imgNum = 0;
//         int batteryLevel = BL.getBatteryChargeLevel();
//         if(batteryLevel >=80){
//           imgNum = 3;
//         } else if(batteryLevel < 80 && batteryLevel >= 50 ){
//           imgNum = 2;
//         } else if(batteryLevel < 50 && batteryLevel >= 20 ){
//           imgNum = 1;
//         } else if(batteryLevel < 20 ){
//           imgNum = 0;
//         }

//         drawingBatteryIcon(batteryImages[imgNum]);    
//         drawingText(String(batteryLevel) + "%");
//         vTaskDelay(1000);
//     }
//       tft.print("Never Used Stack Size: ");
//       tft.println(uxTaskGetStackHighWaterMark(NULL));
//     }
// }

// void drawingBatteryIcon(String filePath){
//    TJpgDec.drawFsJpg(ICON_POS_X, 0, filePath);
// }

// void drawingText(String text){
//   tft.fillRect(0, 0, ICON_POS_X, ICON_HEIGHT,TFT_BLACK);
//   tft.setTextDatum(5);
//   tft.drawString(text, ICON_POS_X-2, STATUS_HEIGHT_BAR/2, 4);
// }

// bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap)
// {
//   if ( y >= tft.height() ) return 0;
//   tft.pushImage(x, y, w, h, bitmap);
//   return 1;
// }

void loop() {
  if (myData.gps_status) {
    GPS_DATA_RECVD_OK = true;
  }
  if (ESP_DATA_RECVD_OK) {
    draw();
  } else {
    draw_no_esp();
  }
  delay(1000);
}
