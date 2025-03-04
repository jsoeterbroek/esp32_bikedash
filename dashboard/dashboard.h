//#ifndef DASHBOARD_H
//#define DASHBOARD_H

#include <fonts/Latin_Hiragana_24.h>
#include <fonts/NotoSansBold15.h>
//#include <fonts/DSEG7.h>
#define latin Latin_Hiragana_24
#define small NotoSansBold15
//#define digits DSEG7_Classic_Bold_32

#define SCREEN_WIDTH TFT_WIDTH
#define SCREEN_HEIGHT TFT_HEIGHT

#define DARK_MODE // TODO: change via programmable button (or screen touch)

#define TEXT_COLOR           TFT_DARKGREY
#define FG_COLOR             TFT_WHITE
#define BG_COLOR             TFT_DARKGREY
#define LINE_COLOR           TFT_LIGHTGREY
#define RECT_FG_COLOR        TFT_DARKGREY
#define RECT_BG_COLOR        TFT_WHITE
#define LEVEL_FG_COLOR       TFT_WHITE
#define LEVEL_BG_COLOR       TFT_DARKGREY

#define NOTIFY_FG_COLOR      TFT_WHITE
#define NOTIFY_BG_COLOR      TFT_ORANGE
#define LEVEL_WARN1_FG_COLOR TFT_ORANGE
#define LEVEL_WARN1_BG_COLOR TFT_ORANGE
#define LEVEL_WARN2_FG_COLOR TFT_RED
#define LEVEL_WARN2_BG_COLOR TFT_RED

#define GAUGE_COLOR 0x055D
#define DATA_COLOR 0x0311
#define PURPLE 0xEA16
#define NEEDLE_COLOR 0xF811

// status flags
bool SETUP_OK = false;
bool ESP_SETUP_OK = false;
bool ESP_DATA_RECVD_OK = false;
bool GPS_OK = false;
bool GPS_DATA_RECVD_OK = false;
bool GSM_OK = false;
bool GSM_DATA_RECVD_OK = false;
bool GSM_DATA_RECVD_AGE_OK = false;
bool TEMP_OK = false;
bool TEMP_DATA_RECVD_OK = false;
bool BATT_OK = false;
bool BATT_DATA_RECVD_OK = false;

