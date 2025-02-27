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
#include <Adafruit_MAX1704X.h>
#include <Wire.h>

#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 320
#define TFT_GREY             0x5AEB   // grey
#define FG_COLOR             0x9986   // red
#define RECT_FG_COLOR        0x5AEB   // grey
#define RECT_BG_COLOR        0xE73C   // white
#define LEVEL_FG_COLOR       0x5AEB   // grey
#define LEVEL_BG_COLOR       0xE73C   // white
#define LEVEL_WARN1_FG_COLOR TFT_ORANGE
#define LEVEL_WARN1_BG_COLOR TFT_ORANGE
#define LEVEL_WARN2_FG_COLOR TFT_RED
#define LEVEL_WARN2_BG_COLOR TFT_RED
#define BG_COLOR             0x5AEB   // grey

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite spr = TFT_eSprite(&tft);

// Battery
#define IP5306_ADDR 0x75
#define MAX17048_ADDR 0x36
#define I2C_SDA 21
#define I2C_SCL 22
Adafruit_MAX17048 maxlipo;
int8_t battery_level = 0;
int8_t old_level = 0;
//bool i2c_supported = false;
//bool has_max17048 = false;
//bool has_ip5306 = false;

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
  delay(2000); // 2 seconds FIXME: remove
  Serial.println("");
  Serial.println("------------- setup start --------------");

  byte error;
  Wire.beginTransmission(MAX17048_ADDR);
  error = Wire.endTransmission();
  if (error == 0) {
    if (maxlipo.begin()) {
      Serial.println("Detected MAX17048");
    }
  }

  //pinoutInit();
  //Serial.print("Value from pin: ");
  //Serial.println(analogRead(ADC_PIN));
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
  tft.setFreeFont(&Orbitron_Medium_20);
  tft.drawString("// BMW R1100GS", 6, 104, GFXFF);
  String software = " esp32_bike ";
  software += String('V') + version_major() + "." + version_minor() + "." + version_patch();
  tft.drawString(software, 6, 190, GFXFF);
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
        _xx = 112; _xx = _xx + step;
        draw_battery_level_box(_xx, _yy, 0, 2);
        step = step + 22;
      }
  } else if (_batt_perc < 21) { 
      for (int i = 1; i<=5 ; i++) {
        _xx = 112; _xx = _xx + step;
        if (i <= 1) { _f = 1; } else { _f = 0; }
        draw_battery_level_box(_xx, _yy, _f, 2);
        step = step + 22;
      }
  } else if (_batt_perc < 41) {
      for (int i = 1; i<=5 ; i++) {
        _xx = 112; _xx = _xx + step;
        if (i <= 2) { _f = 1; } else { _f = 0; }
        draw_battery_level_box(_xx, _yy, _f, 1);
        step = step + 22;
      }
  } else if (_batt_perc < 61) {
      for (int i = 1; i<=5 ; i++) {
        _xx = 112; _xx = _xx + step;
        if (i <= 3) { _f = 1; } else { _f = 0; }
        draw_battery_level_box(_xx, _yy, _f, 0);
        step = step + 22;
      }
  } else if (_batt_perc < 90) {
      for (int i = 1; i<=5 ; i++) {
        _xx = 112; _xx = _xx + step;
        if (i <= 4) { _f = 1; } else { _f = 0; }
        draw_battery_level_box(_xx, _yy, _f, 0);
        step = step + 22;
      }
  } else if (_batt_perc >= 90) {
      for (int i = 1; i<=5 ; i++) {
        _xx = 112; _xx = _xx + step;
        if (i <= 5) { _f = 1; } else { _f = 0; }
        draw_battery_level_box(_xx, _yy, _f, 0);
        step = step + 22;
      }
  }
}

void draw_battery_display_box() {
  int32_t _x = 4; int32_t _y = 28;
  int32_t _w = 232; int32_t _h = 70;
  spr.createSprite(_w, _h);
  spr.fillSprite(TFT_TRANSPARENT);
  // background 
  spr.setTextColor(RECT_BG_COLOR);
  spr.drawRoundRect(0, 0, _w, _h, 10, RECT_BG_COLOR);
  spr.fillRoundRect(1, 1, _w-2, _h-2, 10, RECT_FG_COLOR);
  // label
  spr.setFreeFont(FSS12);
  spr.setTextSize(0);
  //spr.setFreeFont(&Latin_Hiragana_24);

  /////////////////
  // BIKE battery
  int8_t bike_batt_perc = (int)myData.batt_perc;
  // start car battery icon
  spr.fillRect(10, 14, 24, 16, RECT_BG_COLOR);
  spr.fillRect(14, 10, 4, 4, RECT_BG_COLOR);
  spr.fillRect(26, 10, 4, 4, RECT_BG_COLOR);
  spr.drawLine(14, 22, 18, 22, RECT_FG_COLOR);
  spr.drawLine(26, 22, 30, 22, RECT_FG_COLOR);
  spr.drawLine(28, 20, 28, 24, RECT_FG_COLOR);
  // end car battery icon
  spr.drawFloat(myData.batt_volt, 1, 64, 20);
  spr.drawString("v", 96, 20);
  // draw the blocks
  // TEST: uncomment to test (with warning colors)
  // bike_batt_perc = 22;
  draw_battery_display_box_blocks(112, 18, bike_batt_perc);

  /////////////////
  // dashboard battery
  int8_t intern_batt_perc = (int)myData.batt_perc;
   // start battery icon
  spr.fillRect(10, 44, 22, 12, RECT_BG_COLOR);
  spr.fillRect(32, 48, 4, 4, RECT_BG_COLOR);
  // end battery icon
  spr.drawFloat(myData.batt_volt, 1, 64, 46);
  spr.drawString("v", 96, 46);
  // draw the blocks
  // TEST: uncomment to test (with warning colors)
  intern_batt_perc = 22;
  draw_battery_display_box_blocks(112, 44, intern_batt_perc);

  // push and delete sprite
  spr.pushSprite(_x, _y, TFT_TRANSPARENT);
  spr.deleteSprite();
}

void draw_display_box(int32_t _x, int32_t _y, float _display, uint8_t _d, String _display_unit, String _display_label) {
  int32_t _w = 115; int32_t _h = 70;
  spr.createSprite(_w, _h);
  spr.fillSprite(TFT_TRANSPARENT);
  // background 
  spr.setTextColor(RECT_BG_COLOR);
  spr.drawRoundRect(0, 0, _w, _h, 10, RECT_BG_COLOR);
  spr.fillRoundRect(1, 1, _w-2, _h-2, 10, RECT_FG_COLOR);
  // label
  spr.setTextDatum(TL_DATUM);
  spr.setFreeFont(FSSO9);
  spr.drawString(_display_label, 3, 6);
  //spr.drawString(_display_unit, 102, 6);
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
