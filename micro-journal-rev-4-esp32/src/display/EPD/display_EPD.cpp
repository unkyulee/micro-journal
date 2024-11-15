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
    // turn off the board LED
    epd_poweroff_all();
    // epd_poweroff();
}

//
void display_EPD_loop()
{
    static unsigned int last = 0;
    if (millis() - last > 150)
    {
        last = millis();

        JsonDocument &app = app_status();
        static int screen_prev = -1;
        int screen = app["screen"].as<int>();

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
        screen_prev = screen;
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

GFXfont *display_EPD_font()
{
    return (GFXfont *)&monospace;
}

// assign safe rect that constraints inside the range
Rect_t display_rect(int x, int y, int width, int height)
{
    Rect_t area = {
        .x = x,
        .y = y,
        .width = width,
        .height = height,
    };

#ifdef DEBUG
    app_log("Rect x: %d, y: %d, width: %d, height: %d\n", x, y, width, height);
#endif

    // x can't exceed the EPD_WIDTH
    if (area.x + area.width > EPD_WIDTH)
        area.width = EPD_WIDTH - area.x;

    // y can't exceed the EPD_HEIGHT
    if (area.y + area.height > EPD_HEIGHT)
        area.height = EPD_HEIGHT - area.y;

    // width can't be negative
    if (area.width < 0)
        area.width = 0;

    // height can't be negative
    if (area.height < 0)
        area.height = 0;

    return area;
}