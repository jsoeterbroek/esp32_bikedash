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
  gfx->drawRect(8, 8, 60, 40, RGB565_BLACK);
  gfx->drawRect(8, 47, 60, 40, RGB565_BLACK);
  gfx->drawRect(8, 86, 60, 40, RGB565_BLACK);
  gfx->drawRect(8, 125, 60, 40, RGB565_BLACK);
}

void drawdata() {

  gfx->setTextColor(RGB565_BLACK);

  /*****************************************************************************
   * Speed
   ****************************************************************************/
  gfx->setCursor(14, 20);
  gfx->setFont(&FreeMono8pt7b);
  gfx->println("Speed");
  gfx->setCursor(14, 40);
  gfx->setFont(&FreeSansBold10pt7b);
  gfx->println("  70");  

  /*****************************************************************************
   * RPM 
   ****************************************************************************/
  gfx->setCursor(14, 60);
  gfx->setFont(&FreeMono8pt7b);
  gfx->println(" RPM");
  gfx->setCursor(14, 80);
  gfx->setFont(&FreeSansBold10pt7b);
  gfx->println("  33");  

  /*****************************************************************************
   * FUEL 
   ****************************************************************************/
  //gfx->drawRect(8, 86, 60, 40, RGB565_BLACK);
  gfx->fillRect(9, 87, 58, 38, RGB565_ORANGE);
  gfx->setCursor(14, 100);
  gfx->setFont(&FreeMono8pt7b);
  gfx->println("FUEL");
  gfx->setCursor(14, 120);
  gfx->setFont(&FreeSansBold10pt7b);
  gfx->println(" 13%");  
}

void loop() {
  drawscreen();
  drawdata();
  delay(2000); // 2 second
}
