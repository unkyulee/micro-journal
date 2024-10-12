#ifndef BOARD_HAS_PSRAM
#error "Please enable PSRAM, Arduino IDE -> tools -> PSRAM -> OPI !!!"
#endif

#include "display_EPD.h"
#include "../display.h"
#include "app/app.h"

//
#include <Arduino.h>

// screens
#include "GUI/WordProcessor/WordProcessor.h"
#include "GUI/WakeUp/WakeUp.h"
#include "GUI/ErrorScreen/ErrorScreen.h"

// Display Frame Buffer Setup
uint8_t *framebuffer = NULL;
uint8_t *display_EPD_framebuffer()
{
    return framebuffer;
}

//
void display_EPD_setup()
{
    // Initialize Framebuffer
    framebuffer = (uint8_t *)ps_calloc(sizeof(uint8_t), EPD_WIDTH * EPD_HEIGHT / 2);
    if (!framebuffer)
    {
        Serial.println("alloc memory failed !!!");
        while (1)
            ;
    }
    memset(framebuffer, 0xFF, EPD_WIDTH * EPD_HEIGHT / 2);

    // Initialize EPD Screen
    epd_init();
    app_log("E-ink display initialized\n");

    // Turn on the display
    epd_poweron();

    // Clear Screen
    epd_clear();

    // Turn off the display
    epd_poweroff_all(); // turn off the board LED}
    //epd_poweroff();
}

//
void display_EPD_loop()
{
    static unsigned int last = 0;
    if (millis() - last > 200)
    {
        last = millis();

        JsonDocument &app = app_status();
        int screen = app["screen"].as<int>();
        int screen_prev = app["screen_prev"].as<int>();

        // WORD PROCESSOR
        if (screen == WORDPROCESSOR)
        {
            // setup only once
            if (screen != screen_prev)
                WP_setup();
            else
                // loop
                WP_render();
        }
        else if (screen == WAKEUPSCREEN || screen == SLEEPSCREEN)
        {
             // setup only once
            if (screen != screen_prev)
                WakeUp_setup();
            else
                // loop
                WakeUp_render();
        }
        else if (screen == ERRORSCREEN)
        {
             // setup only once
            if (screen != screen_prev)
                ErrorScreen_setup();
            else
                // loop
                ErrorScreen_render();
        }

        //
        app["screen_prev"] = screen;
    }
}

void display_EPD_keyboard(char key)
{
    JsonDocument &app = app_status();
    int screen = app["screen"].as<int>();

    if (screen == WORDPROCESSOR)
    {
        // send the key stroke to word processor
        WP_keyboard(key);
    }
    else if (screen == MENUSCREEN)
    {
        // Menu_keyboard(key);
    }
    else if (screen == ERRORSCREEN)
    {
        ErrorScreen_keyboard(key);
    }
    else if (screen == WAKEUPSCREEN || screen == SLEEPSCREEN)
    {
        WakeUp_keyboard(key);
    }
}