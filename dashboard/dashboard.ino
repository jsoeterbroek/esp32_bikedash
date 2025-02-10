/*******************************************************************************
 ******************************************************************************/

#include <SPI.h>
#include <TFT_eSPI.h>
// #include <NotoSansBold15.h>
#include <NotoSansBold36.h>
// #include <Free_Fonts.h>



/*******************************************************************************
 * Start of TFT_eSPI setting
 ******************************************************************************/
TFT_eSPI tft = TFT_eSPI(); // Create an instance of the display
TFT_eSprite spr = TFT_eSprite(&tft);
// #include <TFT_eSPI_Widgets.h>
// using namespace TFT_eSPI_Widgets;
/*******************************************************************************
 * End of TFT_eSPI setting
 ******************************************************************************/



// colors
#define MY_BACKCOLOR 0x0026
#define MY_GAUGECOLOR 0x055D
#define MY_DATACOLOR 0x0311
#define MY_NEEDLECOLOR 0xF811
#define MY_PURPLE 0xEA16

// font stuff
#define AA_FONT_LARGE NotoSansBold36

void setup(void)
{
  // gfx->begin();
  // gfx->fillScreen(WHITE);

  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);
  ledcAttach(0, 2000, 8);
  ledcWrite(0, 255); /* Screen brightness can be modified by adjusting this parameter. (0-255) */

  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_WHITE);                    // Clear screen
  delay(2000); // 2 seconds
}

void draw_screen() {

  //tft.loadFont(AA_FONT_LARGE);
  tft.setTextDatum(MC_DATUM);                  // Set text datum to middle centre (MC_DATUM)
  tft.setTextColor(TFT_BLACK);

  // outer box
  tft.drawRoundRect(6, 6, 306, 226, 10, TFT_LIGHTGREY);
  // tft.fillRoundRect(6, 6, 306, 226, 10, TFT_LIGHTGREY);

  tft.setCursor(100, 140);
  tft.println(" // BMW R1100 GS ");

  // 1 speed 
  tft.drawRoundRect(12, 22, 102, 100, 10, TFT_LIGHTGREY);

  // 2 RPM
  tft.drawRoundRect(116, 22, 102, 100, 10, TFT_LIGHTGREY);

  // 3 left indic
  tft.drawRect(13, 164, 34, 60, TFT_LIGHTGREY);

  // 4 batt warn
  tft.drawRect(46, 164, 34, 30, TFT_LIGHTGREY);

  // 5 high beam 
  tft.drawRect(46, 193, 34, 31, TFT_LIGHTGREY);

  // 6 fuel warn
  tft.drawRect(79, 164, 34, 60, TFT_LIGHTGREY);

  // 7 neutral warn
  tft.drawRect(112, 164, 34, 60, TFT_LIGHTGREY);

  // 8 oil warn
  tft.drawRect(145, 164, 34, 30, TFT_LIGHTGREY);

  // 9 free (ABS)
  tft.drawRect(145, 193, 34, 31, TFT_LIGHTGREY);

  // 10 right indic
  tft.drawRect(178, 164, 34, 60, TFT_LIGHTGREY);

  //tft.unloadFont(); // Remove the font to recover memory used
}

void draw_data() {

  /*****************************************************************************
   * - 1 - Speed
   ****************************************************************************/

  spr.loadFont(AA_FONT_LARGE); // Must load the font first into the sprite class
  spr.createSprite(60, 60);
  spr.fillSprite(TFT_WHITE);
  // spr.drawRect(0, 0, 60, 60, TFT_BLACK); // Draw sprite border outline (so we see extent)
  spr.setTextColor(TFT_DARKGREY); // Set the sprite font colour
  spr.setTextDatum(MC_DATUM); // Middle Centre datum
  spr.drawString("15", 30, 30 ); // Coords of middle of 60 x 60 Sprite
  spr.pushSprite(30, 40); // Push to TFT screen coord 10, 10
  spr.unloadFont(); // Remove the font from sprite class to recover memory used

  tft.setCursor(47, 100);
  tft.println("KM/h");

  /*****************************************************************************
   * - 2 - RPM 
   ****************************************************************************/
  tft.setCursor(156, 100);
  tft.println("RPM");

  /*****************************************************************************
   * - 3 - LEFT INDICATOR 
   ****************************************************************************/  
  tft.setCursor(20, 200);
  tft.println("Li");

}



void draw_gfx_data() {


  /*****************************************************************************
   * - 6 - FUEL 
   ****************************************************************************/
  // gfx->setTextColor(LIGHTGREY);
  // gfx->setCursor(88, 200);
  // gfx->println("F");

  // demo fuel warn
  // gfx->fillRect(80, 165, 32, 58, ORANGE);
  // gfx->setTextColor(WHITE);
  // gfx->setCursor(88, 200);
  // gfx->println("F");

  /*****************************************************************************
   * - 7 - NEUTRAL INDIC
   ****************************************************************************/
  // gfx->setTextColor(LIGHTGREY);
  // gfx->setCursor(120, 200);
  // gfx->println("N");

  /*****************************************************************************
   * - 10 - RIGHT INDICATOR 
   ****************************************************************************/  


  // demo right indic flashing
  // gfx->fillRect(179, 165, 32, 58, GREEN);
  // gfx->setTextColor(WHITE);
  // gfx->setCursor(184, 200);
  // gfx->println("Ri");
  // delay(1000); // 1 second
  // gfx->fillRect(179, 165, 32, 58, WHITE);
  // gfx->setTextColor(LIGHTGREY);
  // gfx->setCursor(184, 200);
  // gfx->println("Ri");

}

void loop() {
  // draw_gfx_data();
  draw_screen();
  draw_data();
  delay(500); // 1/2 second
}
