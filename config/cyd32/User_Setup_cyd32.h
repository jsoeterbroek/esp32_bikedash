// Setup for the ESP32 CYD32 with ST7789 driver
#define USER_SETUP_INFO "User_Setup"
// See SetupX_Template.h for all options available
#define ST7789_DRIVER

#define TFT_WIDTH  240
#define TFT_HEIGHT 320

#define TFT_RGB_ORDER TFT_BGR 

// If colours are inverted (white shows as black) then uncomment one of the next
// 2 lines try both options, one of the options should correct the inversion.

#define TFT_INVERSION_ON
// #define TFT_INVERSION_OFF

#define TFT_BL    27            // LED back-light control pin
#define TFT_BACKLIGHT_ON HIGH  // Level to turn ON back-light (HIGH or LOW)

#define ESP32_DMA
#define TFT_CS   15 //     10 or 34 (FSPI CS0) 
#define TFT_MOSI 13 //     11 or 35 (FSPI D)
#define TFT_SCLK 14 //     12 or 36 (FSPI CLK)
#define TFT_MISO 12 //     13 or 37 (FSPI Q)
#define TFT_DC    2
#define TFT_RST   35
 //#define TFT_RST   4  // Reset pin (could connect to RST pin)
#define TFT_RST  -1  // Set TFT_RST to -1 if display RESET is connected to ESP32 board RST

//#define TOUCH_CS 33                 // Chip select pin (T_CS) of touch screen
//#define TOUCH_OFFSET_ROTATION	1   // Chip select pin (T_CS) of touch screen


#define LOAD_GLCD   // Font 1. Original Adafruit 8 pixel font needs ~1820 bytes in FLASH
#define LOAD_FONT2  // Font 2. Small 16 pixel high font, needs ~3534 bytes in FLASH, 96 characters
#define LOAD_FONT4  // Font 4. Medium 26 pixel high font, needs ~5848 bytes in FLASH, 96 characters
#define LOAD_FONT6  // Font 6. Large 48 pixel font, needs ~2666 bytes in FLASH, only characters 1234567890:-.apm
#define LOAD_FONT7  // Font 7. 7 segment 48 pixel font, needs ~2438 bytes in FLASH, only characters 1234567890:.
#define LOAD_FONT8  // Font 8. Large 75 pixel font needs ~3256 bytes in FLASH, only characters 1234567890:-.
#define LOAD_GFXFF  // FreeFonts. Include access to the 48 Adafruit_GFX free fonts FF1 to FF48 and custom fonts

#define SMOOTH_FONT

#define SPI_FREQUENCY  80000000   // Maximum for ILI9341
#define SPI_READ_FREQUENCY  10000000 // 6 MHz is the maximum SPI read speed for the ST7789V
#define SPI_TOUCH_FREQUENCY 2500000

//#define USE_HSPI_PORT
