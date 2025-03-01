//#ifndef DASHBOARD_H
//#define DASHBOARD_H

#include <fonts/Latin_Hiragana_24.h>
#include <fonts/NotoSansBold15.h>
//#include <fonts/DSEG7.h>
#define latin Latin_Hiragana_24
#define small NotoSansBold15
//#define digits DSEG7_Classic_Bold_32

#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 320

#define DARK_MODE // TODO: change via programmable button (or screen touch)

#define TEXT_COLOR           TFT_DARKGREY
#define FG_COLOR             TFT_WHITE
#define BG_COLOR             TFT_LIGHTGREY
#define LINE_COLOR           TFT_DARKGREY
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
