/*******************************************************************************
 ******************************************************************************/
#include <Arduino_GFX_Library.h>
#include <FreeMono8pt7b.h>
#include <FreeSansBold10pt7b.h>
#include <FreeSerifBoldItalic12pt7b.h>

/*******************************************************************************
 * Start of Arduino_GFX setting
 ******************************************************************************/
#define GFX_BL DF_GFX_BL // default backlight pin, you may replace DF_GFX_BL to actual backlight pin
#define TFT_BL 27
/* More dev device declaration: https://github.com/moononournation/Arduino_GFX/wiki/Dev-Device-Declaration */
#if defined(DISPLAY_DEV_KIT)
Arduino_GFX *gfx = create_default_Arduino_GFX();
#else /* !defined(DISPLAY_DEV_KIT) */

/* More display class: https://github.com/moononournation/Arduino_GFX/wiki/Display-Class */
Arduino_DataBus *bus = new Arduino_ESP32SPI(2 /* DC */, 15 /* CS */, 14 /* SCK */, 13 /* MOSI */, GFX_NOT_DEFINED /* MISO */);
Arduino_GFX *gfx = new Arduino_ST7789(bus, -1 /* RST */, 1 /* rotation */, true /* IPS */);

#endif /* !defined(DISPLAY_DEV_KIT) */
/*******************************************************************************
 * End of Arduino_GFX setting
 ******************************************************************************/


void setup(void)
{
    gfx->begin();
    gfx->fillScreen(WHITE);

#ifdef TFT_BL
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH);
    ledcAttach(0, 2000, 8);
    ledcWrite(0, 255); /* Screen brightness can be modified by adjusting this parameter. (0-255) */
#endif

    // gfx->setCursor(10, 10);
    // gfx->setTextColor(RED);
    // gfx->println("Setup complete...");
    delay(1000); // 1 seconds
}


void drawscreen() {
  gfx->setFont(&FreeMono8pt7b);
  gfx->drawRect(6, 6, 308, 230, LIGHTGREY);

  // 1 speed 
  gfx->drawRect(12, 22, 102, 100, LIGHTGREY);

  // 2 RPM
  gfx->drawRect(113, 22, 102, 100, LIGHTGREY);

  // logo
  gfx->setTextColor(DARKGREY);
  gfx->setCursor(44, 150);
  gfx->setFont(&FreeSansBold10pt7b);
  gfx->println("// BMW R1100 GS");  

  // 3 left indic
  gfx->drawRect(13, 164, 34, 60, LIGHTGREY);

  // 4 batt warn
  gfx->drawRect(46, 164, 34, 30, LIGHTGREY);

  // 5 high beam 
  gfx->drawRect(46, 193, 34, 31, LIGHTGREY);

  // 6 fuel warn
  gfx->drawRect(79, 164, 34, 60, LIGHTGREY);

  // 7 neutral warn
  gfx->drawRect(112, 164, 34, 60, LIGHTGREY);

  // 8 oil warn
  gfx->drawRect(145, 164, 34, 30, LIGHTGREY);

  // 9 free (ABS)
  gfx->drawRect(145, 193, 34, 31, LIGHTGREY);

  // 10 right indic
  gfx->drawRect(178, 164, 34, 60, LIGHTGREY);

}

void drawdata() {

  gfx->setTextColor(RGB565_BLACK);

  /*****************************************************************************
   * - 1 - Speed
   ****************************************************************************/
  gfx->setFont(&FreeSerifBoldItalic12pt7b);
  gfx->setTextColor(DARKGREY);
  gfx->setCursor(48, 80);
  gfx->println("56");  
  gfx->setCursor(40, 114);
  gfx->setFont(&FreeMono8pt7b);
  gfx->setTextColor(LIGHTGREY);
  gfx->println("KM/h");

  /*****************************************************************************
   * - 2 - RPM 
   ****************************************************************************/
  // gfx->setCursor(14, 80);
  // gfx->setFont(&FreeSansBold10pt7b);
  // gfx->println("  33");  
  gfx->setCursor(148, 114);
  gfx->setTextColor(LIGHTGREY);
  gfx->setFont(&FreeMono8pt7b);
  gfx->println("RPM");


  /*****************************************************************************
   * - 3 - LEFT INDICATOR 
   ****************************************************************************/  
  gfx->setFont(&FreeSansBold10pt7b);
  gfx->setTextColor(LIGHTGREY);
  gfx->setCursor(20, 200);
  gfx->println("Li");

  /*****************************************************************************
   * - 6 - FUEL 
   ****************************************************************************/
  gfx->setFont(&FreeSansBold10pt7b);
  gfx->setTextColor(LIGHTGREY);
  gfx->setCursor(88, 200);
  gfx->println("F");

  // demo fuel warn
  gfx->fillRect(80, 165, 32, 58, ORANGE);
  gfx->setTextColor(WHITE);
  gfx->setCursor(88, 200);
  gfx->println("F");

  /*****************************************************************************
   * - 7 - NEUTRAL INDIC
   ****************************************************************************/
  gfx->setFont(&FreeSansBold10pt7b);
  gfx->setTextColor(LIGHTGREY);
  gfx->setCursor(120, 200);
  gfx->println("N");

  /*****************************************************************************
   * - 10 - RIGHT INDICATOR 
   ****************************************************************************/  

  gfx->setFont(&FreeSansBold10pt7b);

  // demo right indic flashing
  gfx->fillRect(179, 165, 32, 58, GREEN);
  gfx->setTextColor(WHITE);
  gfx->setCursor(184, 200);
  gfx->println("Ri");
  delay(1000); // 1 second
  gfx->fillRect(179, 165, 32, 58, WHITE);
  gfx->setTextColor(LIGHTGREY);
  gfx->setCursor(184, 200);
  gfx->println("Ri");

}

void loop() {
  drawscreen();
  drawdata();
  delay(500); // 1/2 second
}
