#include "display_EPD.h"
#include "app/app.h"

//
#include "epd_driver.h"
uint8_t *framebuffer = NULL;

//
void display_EPD_setup()
{

    framebuffer = (uint8_t *)ps_calloc(sizeof(uint8_t), EPD_WIDTH * EPD_HEIGHT / 2);
    if (!framebuffer)
    {
        Serial.println("alloc memory failed !!!");
        while (1)
            ;
    }
    memset(framebuffer, 0xFF, EPD_WIDTH * EPD_HEIGHT / 2);

    epd_init();

    Rect_t area = {
        .x = 230,
        .y = 20,
        .width = 100,
        .height = 100,
    };

    epd_poweron();
    epd_clear();
    epd_draw_hline(10, random(10, EPD_HEIGHT), EPD_WIDTH - 20, 0, framebuffer);
    // epd_draw_grayscale_image(area, (uint8_t *)logo_data);
    // epd_draw_image(area, (uint8_t *)logo_data, BLACK_ON_WHITE);
    // epd_poweroff();
    epd_poweroff_all();
    app_log("EPD\n");
}

//
void display_EPD_loop()
{
    /*
    static unsigned int last = millis();
    if (millis() - last > 5000)
    {
        last = millis();

        //
        epd_poweron();
        epd_clear();

        //
        epd_draw_hline(10, random(10, EPD_HEIGHT), EPD_WIDTH - 20, 0, framebuffer);


        app_log("Off\n");
    }
    */
}

void display_EPD_keyboard(char key)
{
}