//#ifndef DASHBOARD_H
//#define DASHBOARD_H

#include <Latin_Hiragana_24.h>
#include <NotoSansBold15.h>
//#include <fonts/DSEG7.h>
#define latin Latin_Hiragana_24
#define small NotoSansBold15
//#define digits DSEG7_Classic_Bold_32

#define SCREEN_WIDTH TFT_WIDTH
#define SCREEN_HEIGHT TFT_HEIGHT

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
bool BIKE_LOCKED = true;