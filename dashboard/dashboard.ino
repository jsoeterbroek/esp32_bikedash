/*******************************************************************************
 ******************************************************************************/
#include <../common/common.h>
#include <esp_now.h>
#include <WiFi.h>
#include <TFT_eSPI.h>
//#include <fonts/Free_Fonts.h>
//#include <fonts/Orbitron_Medium_20.h>
#include <fonts/Latin_Hiragana_24.h>
#include <fonts/NotoSansBold15.h>
#include <fonts/DSEG7.h>

#define latin Latin_Hiragana_24
#define small NotoSansBold15
#define digits DSEG7_Classic_Bold_32

#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 320
#define FG_COLOR             TFT_WHITE
#define BG_COLOR             TFT_LIGHTGREY
#define RECT_LINE_COLOR      TFT_DARKGREY
#define RECT_FG_COLOR        TFT_DARKGREY
#define RECT_BG_COLOR        TFT_WHITE
#define LEVEL_FG_COLOR       TFT_WHITE
#define LEVEL_BG_COLOR       TFT_DARKGREY
#define LEVEL_WARN1_FG_COLOR TFT_ORANGE
#define LEVEL_WARN1_BG_COLOR TFT_ORANGE
#define LEVEL_WARN2_FG_COLOR TFT_RED
#define LEVEL_WARN2_BG_COLOR TFT_RED

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite spr = TFT_eSprite(&tft);

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
  delay(4000); // 4 seconds
  Serial.println("");
  Serial.println("------------- setup start --------------");

  tft.init();
  tft.setRotation(0);
  tft.fillScreen(BG_COLOR);

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
  tft.loadFont(latin);
  tft.drawString("// BMW R1100GS", 6, 104);
  String software = " esp32_bike ";
  software += String('V') + version_major() + "." + version_minor() + "." + version_patch();
  tft.drawString(software, 6, 190);
  tft.unloadFont();
  delay(6000); // 6 seconds
  Serial.println("------------- setup end --------------");
  tft.fillScreen(BG_COLOR);            // Clear screen
}

void draw_battery_level_box(int32_t _lx, int32_t _ly, bool _filled, int8_t _warn_level) {

  int32_t _lw = 20; int32_t _lh = 10;

  int fg_color = LEVEL_FG_COLOR;
  int bg_color = LEVEL_BG_COLOR;

  switch (_warn_level) {
    case 1:
      bg_color = LEVEL_WARN1_BG_COLOR;
      break;
    case 2:
      bg_color = LEVEL_WARN2_BG_COLOR;
      break;
  }

  spr.drawRect(_lx, _ly, _lw, _lh, bg_color);

  if (_filled) {
    spr.fillRect(_lx+1, _ly+1, _lw-2, _lh-2, bg_color);
  } else {
    spr.fillRect(_lx+1, _ly+1, _lw-2, _lh-2, fg_color);
  }
}

void draw_battery_display_box_blocks(int32_t _xx, int32_t _yy, int8_t _batt_perc) {

  int8_t _warn = 0;
  // calculate battery percentage
  int8_t _c = 0; int8_t _f = 0; //filled blocks
  uint8_t step = 0;

  if (_batt_perc < 5) { 
    for (int i = 1; i<=5 ; i++) {
      _xx = 116; _xx = _xx + step;
      draw_battery_level_box(_xx, _yy, 0, 2);
      step = step + 22;
    }
  } else if (_batt_perc < 21) { 
    for (int i = 1; i<=5 ; i++) {
      _xx = 116; _xx = _xx + step;
      if (i <= 1) { _f = 1; } else { _f = 0; }
      draw_battery_level_box(_xx, _yy, _f, 2);
      step = step + 22;
    }
  } else if (_batt_perc < 41) {
    for (int i = 1; i<=5 ; i++) {
      _xx = 116; _xx = _xx + step;
      if (i <= 2) { _f = 1; } else { _f = 0; }
      draw_battery_level_box(_xx, _yy, _f, 1);
      step = step + 22;
    }
  } else if (_batt_perc < 61) {
    for (int i = 1; i<=5 ; i++) {
      _xx = 116; _xx = _xx + step;
      if (i <= 3) { _f = 1; } else { _f = 0; }
      draw_battery_level_box(_xx, _yy, _f, 0);
      step = step + 22;
    }
  } else if (_batt_perc < 90) {
    for (int i = 1; i<=5 ; i++) {
      _xx = 116; _xx = _xx + step;
      if (i <= 4) { _f = 1; } else { _f = 0; }
      draw_battery_level_box(_xx, _yy, _f, 0);
      step = step + 22;
    }
  } else if (_batt_perc >= 90) {
    for (int i = 1; i<=5 ; i++) {
      _xx = 116; _xx = _xx + step;
      if (i <= 5) { _f = 1; } else { _f = 0; }
      draw_battery_level_box(_xx, _yy, _f, 0);
      step = step + 22;
    }
  }
}

void draw_battery_display_box() {
  int32_t _x = 4; int32_t _y = 32;
  int32_t _w = 232; int32_t _h = 66;
  spr.createSprite(_w, _h);
  spr.fillSprite(TFT_TRANSPARENT);
  // background 
  spr.setTextColor(RECT_FG_COLOR);
  spr.drawRoundRect(0, 0, _w, _h, 10, RECT_LINE_COLOR);
  spr.fillRoundRect(1, 1, _w-2, _h-2, 10, RECT_BG_COLOR);
  // label
  //spr.setFreeFont(&Latin_Hiragana_24);

  /////////////////
  // BIKE battery
  int8_t bike_batt_perc = (int)myData.batt_perc;
  // start car battery icon
  spr.fillRect(10, 32, 24, 16, RECT_FG_COLOR);
  spr.fillRect(14, 28, 4, 4, RECT_FG_COLOR);
  spr.fillRect(26, 28, 4, 4, RECT_FG_COLOR);
  spr.drawLine(14, 40, 18, 40, RECT_BG_COLOR);
  spr.drawLine(26, 40, 30, 40, RECT_BG_COLOR);
  spr.drawLine(28, 38, 28, 42, RECT_BG_COLOR);
  // end car battery icon
  spr.loadFont(latin);
  spr.setTextSize(1);
  spr.drawFloat(myData.batt_volt, 1, 68, 40);
  spr.setTextSize(0);
  spr.drawString("v", 100, 40);
  spr.unloadFont();
  // draw the blocks
  // TEST: uncomment to test (with warning colors)
  // bike_batt_perc = 69;
  draw_battery_display_box_blocks(116, 34, bike_batt_perc);

  // push and delete sprite
  spr.pushSprite(_x, _y, TFT_TRANSPARENT);
  spr.deleteSprite();
}

void draw_display_box(int32_t _x, int32_t _y, float _display, uint8_t _d, String _display_unit, String _display_label) {
  int32_t _w = 115; int32_t _h = 70;
  spr.createSprite(_w, _h);
  spr.fillSprite(TFT_TRANSPARENT);
  // background 
  spr.setTextColor(RECT_FG_COLOR);
  spr.drawRoundRect(0, 0, _w, _h, 10, RECT_LINE_COLOR);
  spr.fillRoundRect(1, 1, _w-2, _h-2, 10, RECT_BG_COLOR);
  // label
  spr.setTextDatum(TL_DATUM);
  spr.loadFont(small);
  spr.drawString(_display_label, 6, 6);
  //spr.drawString(_display_unit, 102, 6);
  // main display number
  spr.unloadFont();
  spr.setTextDatum(MC_DATUM);
  spr.setFreeFont(&DSEG7_Classic_Bold_32);
  spr.drawFloat(_display, _d, _w / 2, 46);
  // push and delete sprite
  spr.pushSprite(_x, _y, TFT_TRANSPARENT);
  spr.deleteSprite();
}

void draw_no_esp() {

  // TODO: once design of main screen is settled, build this screen
}

void draw() {

  // TODO: night and day display,
  // during day light background, dark foreground
  // during night dark background, light foreground
  tft.setTextColor(TFT_WHITE);
  tft.loadFont(latin);
  tft.setTextSize(0);

  tft.fillRect(0, 0, 280, 28, TFT_ORANGE);

  // GPS Time 
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
    tft.drawString(gps_time, 88, 4);
  }
  tft.unloadFont();

  tft.setTextColor(FG_COLOR);
  int32_t x = 0; int32_t y = 0; int32_t r = 10;
  int32_t t = 0; int32_t w = 0; int32_t h = 0;

  /*****************************************************************************
   * Bike Battery & Li-ion battery
   ****************************************************************************/
  draw_battery_display_box();
  /*****************************************************************************
   * Temp & humidity 
   ****************************************************************************/
  draw_display_box(4, 100, myData.temp, 1, "C", "Temperature");
  draw_display_box(122, 100, myData.hum, 1, "%", "Humidity");
  /*****************************************************************************
   * Fuel
   ****************************************************************************/  
  draw_display_box(4, 172, 0, 0, " ", "Free");
  draw_display_box(122, 172, myData.fuel_perc, 0, "%", "Fuel");
  /*****************************************************************************
   * GPS Altitude & ground speed
   ****************************************************************************/
  draw_display_box(4, 244, myData.gps_altitude_meters, 1, "m", "Altitude");
  draw_display_box(122, 244, myData.gps_speed_kmph, 0, "Km", "Speed");
}

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
