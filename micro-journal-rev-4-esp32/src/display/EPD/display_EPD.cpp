#ifndef BOARD_HAS_PSRAM
#error "Please enable PSRAM, Arduino IDE -> tools -> PSRAM -> OPI !!!"
#endif

#include "app/app.h"

#include <Arduino.h>
#include "epd_driver.h"
#include "utilities.h"

uint8_t *framebuffer = NULL;

//
void display_EPD_setup()
{
    //
    app_log("EPD Initializing\n");

    //
    framebuffer = (uint8_t *)ps_calloc(sizeof(uint8_t), EPD_WIDTH * EPD_HEIGHT / 2);
    if (!framebuffer)
    {
        Serial.println("alloc memory failed !!!");
        while (1)
            ;
    }
    memset(framebuffer, 0xFF, EPD_WIDTH * EPD_HEIGHT / 2);

    epd_init();

    epd_poweron();
    epd_clear();
    epd_poweroff();
}

//
void display_EPD_loop()
{
    static unsigned int last = millis();
    if (millis() - last > 5000)
    {
        //
        last = millis();

        app_log("EPD Loop\n");

        epd_poweron();
        epd_draw_hline(10, random(10, EPD_HEIGHT), EPD_WIDTH - 20, 0, framebuffer);
        epd_draw_grayscale_image(epd_full_screen(), framebuffer);
        
        memset(framebuffer, 0xFF, EPD_WIDTH * EPD_HEIGHT / 2);
        epd_clear();
        epd_poweroff();
    }
}

void display_EPD_keyboard(char key)
{
}