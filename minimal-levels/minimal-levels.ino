#include <common.h>
#include <TFT_eSPI.h>
#include <TFT_eWidget.h>
#include <NotoSansBold36.h>
TFT_eSPI tft = TFT_eSPI();
//TFT_eSprite needle = TFT_eSprite(&tft); // Sprite object for needle
//TFT_eSprite spr    = TFT_eSprite(&tft); // Sprite for meter reading
MeterWidget levels_meter = MeterWidget(&tft);

#define LOOP_PERIOD 35 // Display updates every 35 ms

//............................dont edit this
int cx=75;
int cy=75;
int r=72;
int ir=70;
int n=0;
int angle=0;

float x[360]; //outer points of Speed gauges
float y[360];
float px[360]; //ineer point of Speed gauges
float py[360];
float lx[360]; //text of Speed gauges
float ly[360];
float nx[360]; //needle low of Speed gauges
float ny[360];
float x2[360]; //outer points of RPM gauges
float y2[360];
float px2[360]; //inner point of RPM gauges
float py2[360];
float lx2[360]; //text of RPM gauges
float ly2[360];
float nx2[360]; //needle low of RPM gauges
float ny2[360];

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

int gearMaxSpeed[8]={12,0,60,90,120,150,190,236};
String gears[8]={"R","N","1","2","3","4","5","6"};
int selectedGear=1;

//........................................................colors
unsigned short blockColor[4]={0x0312,0x0290,0x01EC,0x016A};
unsigned short dirColor[2]={0x0312,TFT_ORANGE};
unsigned short lightColor[3]={0x01EC,0x0FA8,0xB79F};

// .........important variables
bool leftPointer=0; 
bool rightPointer=0;
bool braking;
int lights=0;  //0 is lights off, 1 is short light, 2 is long lights
float speedAngle=0; //...speed variable 0-240
float rpmAngle=5;  //.....RPM variable 0-9

float mapValue(float ip, float ipmin, float ipmax, float tomin, float tomax)
{
  return tomin + (((tomax - tomin) * (ip - ipmin))/ (ipmax - ipmin));
}

void setup(){

  tft.init();

  tft.setRotation(0);
  tft.fillScreen(backColor);
  Serial.begin(115200); // For debug
  // spr.createSprite(150,150);
  levels_meter.analogMeter(0, 256, 100, "R", "0", "", "50", "", "100"); // Draw analogue meter at 0, 128

}

void draw(){
  static int d = 0;
  static uint32_t updateTime = 0;  

  if (millis() - updateTime >= LOOP_PERIOD) 
  {
    updateTime = millis();

    d += 4; if (d > 360) d = 0;

    // Create a Sine wave for testing, value is in range 0 - 100
    float value = 50.0 + 50.0 * sin((d + 0) * 0.0174532925);

    float demo_speed;
    demo_speed = mapValue(value, (float)0.0, (float)100.0, (float)0.0, (float)10.0);
    levels_meter.updateNeedle(demo_speed, 0);
  }
}

void loop(){

  draw();

}
