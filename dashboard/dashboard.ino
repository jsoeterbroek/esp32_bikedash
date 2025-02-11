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
TFT_eSprite sprite = TFT_eSprite(&tft);
// #include <TFT_eSPI_Widgets.h>
// using namespace TFT_eSPI_Widgets;
/*******************************************************************************
 * End of TFT_eSPI setting
 ******************************************************************************/


//............INPUT PINS............switches and buttons
// #define THROTTLE 43
// #define BRAKE 44
// #define LEFT  17  //direction pointer
// #define RIGHT 18  //direction ponter
// #define SHORT 16  //headlights
// #define LONG  21 //headlights light
// #define GEARUP 12
// #define GEARDOWN 13
// #define HORN 10
// #define BRIGHTNESS 14



// colors
#define backColor 0x0026
#define gaugeColor 0x055D
#define dataColor 0x0311
#define purple 0xEA16
#define needleColor 0xF811
unsigned short blockColor[4]={0x0312,0x0290,0x01EC,0x016A};
unsigned short dirColor[2]={0x0312,TFT_ORANGE};
unsigned short lightColor[3]={0x01EC,0x0FA8,0xB79F};

// test vars
boolean test_LEFT=0;

// font stuff
#define AA_FONT_LARGE NotoSansBold36

double rad=0.01745;
unsigned short color1;
unsigned short color2;
float sA;
float rA;
int blinkPeriod=500;
unsigned long currentTimeL=0;
unsigned long currentTimeR=0;
int brightnesses[5]={40,80,120,150,240};
int selectedBrightness=3;
int deb1=0;
int deb2=0;
int debB=0;

// .........important variables
bool leftPointer=0; 
bool rightPointer=0;
bool braking;
int lights=0;  //0 is lights off, 1 is short light, 2 is long lights
float speedAngle=0; //...speed variable 0-240
float rpmAngle=5;  //.....RPM variable 0-9

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
  tft.fillScreen(TFT_DARKGREY);                    // Clear screen
  delay(2000); // 2 seconds
}

void draw_dash() {

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

void draw() {

  sprite.createSprite(300, 220);

  //tft.drawRoundRect(6, 6, 306, 226, 10, TFT_LIGHTGREY);
  //sprite.drawRoundRect(6, 6, 306, 226, 10, TFT_LIGHTGREY);
  //sprite.fillSprite(backColor);
  sprite.fillSprite(TFT_TRANSPARENT);
  //sprite.drawRect(0, 0, 300, 300, TFT_BLACK); // Draw sprite border outline (so we see extent)
  for(int i=0;i<4;i++) {
    sprite.fillRect(120,28+i*24,80,22,blockColor[i]);
  }

  /*****************************************************************************
   * - 1 - Speed
   ****************************************************************************/

  // sprite.loadFont(AA_FONT_LARGE); // Must load the font first into the sprite class
  // sprite.createSprite(60, 60);
  // sprite.fillSprite(TFT_WHITE);
  // spr.drawRect(0, 0, 60, 60, TFT_BLACK); // Draw sprite border outline (so we see extent)
  // sprite.setTextColor(TFT_DARKGREY); // Set the sprite font colour
  // sprite.setTextDatum(MC_DATUM); // Middle Centre datum
  // sprite.drawString("15", 30, 30 ); // Coords of middle of 60 x 60 Sprite
  // sprite.pushSprite(30, 40); // Push to TFT screen coord 10, 10
  // sprite.unloadFont(); // Remove the font from sprite class to recover memory used

  // tft.setCursor(47, 100);
  // tft.println("KM/h");

  /*****************************************************************************
   * - 2 - RPM 
   ****************************************************************************/
  // tft.setCursor(156, 100);
  // tft.println("RPM");

  /*****************************************************************************
   * - 3 - LEFT INDICATOR 
   ****************************************************************************/  
  // tft.setCursor(20, 200);
  // tft.println("Li");

  sprite.fillTriangle(126,14,136,7,136,21,dirColor[leftPointer]);   //direction pointers
  sprite.fillRect(136,11,8,7,dirColor[leftPointer]);
  sprite.fillTriangle(126+68,14,136+48,7,136+48,21,dirColor[rightPointer]);
  sprite.fillRect(176,11,8,7,dirColor[rightPointer]);

  sprite.pushSprite(0,10);
}



int blinking=1;

void loop() {

  draw_dash();
  // if(digitalRead(LEFT)==0) {
  if(test_LEFT==0){
    if(millis()>currentTimeL+blinkPeriod) {
      leftPointer=!leftPointer;
      // digitalWrite(left_pointer,leftPointer);
      currentTimeL=millis();
    }
  } else {
    leftPointer=0;
    // digitalWrite(left_pointer,leftPointer);
  }

  // if(digitalRead(RIGHT)==0) {
  //   if(millis()>currentTimeR+blinkPeriod) {
  //      rightPointer=!rightPointer;
  // digitalWrite(right_pointer,rightPointer);
  //    currentTimeR=millis();
  //  }
  //} else {
  //  rightPointer=0;
  //  digitalWrite(right_pointer,rightPointer);
  //}

  draw();
  delay(500); // 1/2 second
}
