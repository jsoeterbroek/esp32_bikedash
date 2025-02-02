#include <lvgl.h>
#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI(); /* TFT instance */

void setup()
{


    lv_init();

    tft.begin(); /* TFT init */
    tft.setRotation(1); /* Landscape orientation */

}

void loop()
{

    lv_task_handler(); /* let the GUI do its work */
    delay(5);
}
