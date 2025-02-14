/*******************************************************************************
 ******************************************************************************/
#include <SPI.h>
#include <esp_now.h>
#include <WiFi.h>
#include <Arduino_GFX_Library.h>
// #include <NotoSansBold15.h>
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

//Structure example to send data
//Must match the receiver structure
typedef struct struct_message {
    int id;
    float temp;
    float hum;
    unsigned int readingId;
    uint8_t speed_kph;
    uint8_t speed_rpm;
    int8_t fuel_perc;
} struct_message;

// Create a struct_message to hold incoming data
struct_message myData;

//callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.print("temp: ");
  Serial.println(myData.temp);  
  Serial.print("hum: ");
  Serial.println(myData.hum);
}

void setup() {
    //Initialize Serial Monitor
    Serial.begin(115200);

    //Set device as a Wi-Fi Station
    WiFi.mode(WIFI_STA);

    //Init ESP-NOW
    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
        return;
    }

    // Once ESPNow is successfully Init, we will register for recv CB to
    // get recv packer info
    esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));

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

void loop() {
    gfx->fillScreen(WHITE);
    draw();
    gfx->flush();
    delay(2000);
}

void draw() {

    gfx->setTextColor(RGB565_BLACK);

    /*****************************************************************************
     * Speed
     ****************************************************************************/
    gfx->drawRect(8, 10, 60, 40, RGB565_BLACK);
    gfx->setCursor(14, 22);
    gfx->setFont(&FreeMono8pt7b);
    gfx->println("Speed");
    gfx->setCursor(14, 42);
    gfx->setFont(&FreeSansBold10pt7b);
    gfx->println("  70");  

    /*****************************************************************************
     * RPM 
     ****************************************************************************/
    gfx->drawRect(8, 49, 60, 40, RGB565_BLACK);
    gfx->setCursor(14, 62);
    gfx->setFont(&FreeMono8pt7b);
    gfx->println(" RPM");
    gfx->setCursor(14, 82);
    gfx->setFont(&FreeSansBold10pt7b);
    gfx->println("  33");  

    /*****************************************************************************
     * Temp 
     ****************************************************************************/
    gfx->drawRect(8, 88, 60, 40, RGB565_BLACK);
    gfx->setCursor(14, 102);
    gfx->setFont(&FreeMono8pt7b);
    gfx->println(" Temp");
    gfx->setCursor(14, 122);
    gfx->setFont(&FreeSansBold10pt7b);
    gfx->println(myData.temp);  
}
