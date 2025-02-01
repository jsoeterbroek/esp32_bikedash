#include <lvgl.h>
#include <TFT_eSPI.h>
#include <ui.h>

/* lvgl version 8.2
/*Don't forget to set Sketchbook location in File/Preferences to the path of your UI project (the parent foder of this INO file)*/

/*Change to your screen resolution*/
static const uint16_t screenWidth  = 480;
static const uint16_t screenHeight = 320;

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[ screenWidth * screenHeight / 10 ];

TFT_eSPI tft = TFT_eSPI(screenWidth, screenHeight); /* TFT instance */

#if LV_USE_LOG != 0
/* Serial debugging */
void my_print(const char * buf)
{
    Serial.printf(buf);
    Serial.flush();
}
#endif
//inputs analog values
int speed = 0;          // Current speed value
int fuel = 0;          // Current fuel value
int temp = 0;          // Current temp value
int battery = 0;          // Current battery value
int pressure = 0;          // Current pressure value

/* Display flushing */
void my_disp_flush( lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p )
{
    uint32_t w = ( area->x2 - area->x1 + 1 );
    uint32_t h = ( area->y2 - area->y1 + 1 );

    tft.startWrite();
    tft.setAddrWindow( area->x1, area->y1, w, h );
    tft.pushColors( ( uint16_t * )&color_p->full, w * h, true );
    tft.endWrite();

    lv_disp_flush_ready( disp );
}

/*Read the touchpad*/
void my_touchpad_read( lv_indev_drv_t * indev_driver, lv_indev_data_t * data )
{
    uint16_t touchX = 0, touchY = 0;

    bool touched = false;//tft.getTouch( &touchX, &touchY, 600 );

    if( !touched )
    {
        data->state = LV_INDEV_STATE_REL;
    }
    else
    {
        data->state = LV_INDEV_STATE_PR;

        /*Set the coordinates*/
        data->point.x = touchX;
        data->point.y = touchY;

        Serial.print( "Data x " );
        Serial.println( touchX );

        Serial.print( "Data y " );
        Serial.println( touchY );
    }
}

void setup()
{
    Serial.begin( 115200 ); /* prepare for possible serial debug */

    String LVGL_Arduino = "Hello Arduino! ";
    LVGL_Arduino += String('V') + lv_version_major() + "." + lv_version_minor() + "." + lv_version_patch();

    Serial.println( LVGL_Arduino );
    Serial.println( "I am LVGL_Arduino" );

    lv_init();

#if LV_USE_LOG != 0
    lv_log_register_print_cb( my_print ); /* register print function for debugging */
#endif

    tft.begin();          /* TFT init */
    tft.setRotation( 3 ); /* Landscape orientation, flipped */

    lv_disp_draw_buf_init( &draw_buf, buf, NULL, screenWidth * screenHeight / 10 );

    /*Initialize the display*/
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init( &disp_drv );
    /*Change the following line to your display resolution*/
    disp_drv.hor_res = screenWidth;
    disp_drv.ver_res = screenHeight;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register( &disp_drv );

    /*Initialize the (dummy) input device driver*/
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init( &indev_drv );
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = my_touchpad_read;
    lv_indev_drv_register( &indev_drv );


    ui_init();

    Serial.println( "Setup done" );
}

void loop()
{
    lv_timer_handler(); /* let the GUI do its work */
    delay(5);

        // Update speed label with the current value
    char rpm_str[10];
    sprintf(rpm_str, "%d", speed);  // Display the current RPM value (e.g., 100, 200, etc.)
    lv_label_set_text(ui_Labelkm, rpm_str);
// am using demo purpose all the input in analog 12 please change each input to eash analog
    // Update gauge needle based on Speed value
    // Map 0 Speed to 0 degrees and 200 km to 3600 degrees
    int speed = map(analogRead(12), 0, 4096, 0, 200); // change analog pin as you desired
    int Needle1R = map(speed, 0, 200, 300, 3300); 
    lv_img_set_angle(ui_Needle1, Needle1R);
    _ui_label_set_property(ui_Labelkm, _UI_LABEL_PROPERTY_TEXT, String(speed).c_str());

    // Update gauge needle based on Fuel value
    int fuel = map(analogRead(12), 0, 4096, 0, 100); // change analog pin as you desired
    int Needle2R = map(fuel, 0, 100, 1500, 3000); 
    lv_img_set_angle(ui_Needle2, Needle2R);

    // Update gauge needle based on temp value
    int temp = map(analogRead(12), 0, 4096, 0, 100); // change analog pin as you desired
    int Needle3R = map(temp, 0, 100, 600, 2000); 
    lv_img_set_angle(ui_Needle3, Needle3R);

    // Update gauge needle based on battery value
    int battery = map(analogRead(12), 0, 4096, 0, 100); // change analog pin as you desired
    int Needle4R = map(battery, 0, 100, 1850, 2350); 
    lv_img_set_angle(ui_Needle4, Needle4R);

        // Update gauge needle based on Fuel value
    int pressure = map(analogRead(12), 0, 4096, 0, 100); // change analog pin as you desired
    int Needle5R = map(pressure, 0, 100, 1250, 1750); 
    lv_img_set_angle(ui_Needle5, Needle5R);
}
