#include "display_EPD.h"
#include "../display.h"
#include "app/app.h"

//
#include <Arduino.h>

// screens
#include "ErrorScreen/ErrorScreen.h"
#include "WakeUp/WakeUp.h"
#include "WordProcessor/WordProcessor.h"
#include "Sleep/Sleep.h"
#include "Menu/Menu.h"

// Display Frame Buffer Setup
uint8_t *framebuffer = NULL;
uint8_t *display_EPD_framebuffer()
{
    return framebuffer;
}

int display_EPD_core()
{
    JsonDocument &app = status();
    int screen = app["screen"].as<int>();

    // IN MENU run it in the main core, so that sync process runs independently
    if (screen == MENUSCREEN)
        return 1;

    // by default run at the second core separated from keyboard loop
    return 0;
}

void display_draw_buffer()
{
    epd_poweron();
    epd_draw_grayscale_image(epd_full_screen(), display_EPD_framebuffer());
    memset(display_EPD_framebuffer(), 0xFF, EPD_WIDTH * EPD_HEIGHT / 2);
    epd_poweroff_all();
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
    _log("E-ink display initialized\n");
}

//
void display_EPD_loop()
{
    // process display
    static unsigned int last = 0;
    if (millis() - last > 150)
    {
        last = millis();

        JsonDocument &app = status();
        static int screen_prev = -1;
        int screen = app["screen"].as<int>();

        // Error Screen
        if (screen == ERRORSCREEN)
        {
            // setup only once
            if (screen != screen_prev)
                ErrorScreen_setup();
            else
                // loop
                ErrorScreen_render();
        }

        // WAKE UP SCREEN
        // does nothing and skips immediately to WORDPROCESSOR
        else if (screen == WAKEUPSCREEN)
        {
            // setup only once
            if (screen != screen_prev)
                WakeUp_setup();
            else
                // loop
                WakeUp_render();
        }

        // WORD PROCESSOR
        else if (screen == WORDPROCESSOR)
        {
            // setup only once
            if (screen != screen_prev)
                WP_setup();
            else
                // loop
                WP_render();
        }

        else if (screen == SLEEPSCREEN)
        {
            // setup only once
            if (screen != screen_prev)
                Sleep_setup();
            else
                // loop
                Sleep_render();
        }

        // MENU SCREEN
        else if (screen == MENUSCREEN)
        {
            // setup only once
            if (screen != screen_prev)
                Menu_setup();
            else
                // loop
                Menu_render();
        }

        //
        app["screen_prev"] = screen;
        screen_prev = screen;
    }
}

void display_EPD_keyboard(char key, bool pressed, int index)
{
    JsonDocument &app = status();
    int screen = app["screen"].as<int>();

    if (screen == ERRORSCREEN)
    {
        if (!pressed)
            ErrorScreen_keyboard(key);
    }

    else if (screen == WAKEUPSCREEN)
    {
        if (!pressed)
            WakeUp_keyboard(key);
    }

    else if (screen == WORDPROCESSOR)
    {
        // send the key stroke to word processor
        WP_keyboard(key, pressed, index);
    }

    else if (screen == SLEEPSCREEN)
    {
        if (!pressed)
            Sleep_keyboard(key);
    }

    else if (screen == MENUSCREEN)
    {
        if (!pressed)
            Menu_keyboard(key);
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

    //
    // _debug("display_rect:: x: %d, y: %d, width: %d, height: %d\n", x, y, width, height);

    return area;
}

// DISPLAY CONFIGURATION
int _x_init = 10;
int _y_init = 10;
int _line_height = 50;
int _x_pos = 0;
int _y_pos = 0;
int _font_width = 30;

void display_initialize(int x_init, int y_init, int line_height, int font_width)
{
    //
    _x_init = x_init;
    _y_init = y_init;
    _line_height = line_height;
    _font_width = font_width;

    //
    _x_pos = x_init;
    _y_pos = y_init + _line_height;
}

void display_newline()
{
    _x_pos = _x_init;
    _y_pos += _line_height;
}

void display_setline(int row)
{
    _x_pos = _x_init;
    _y_pos = _y_init + _line_height * (row);
}

int display_x()
{
    return _x_pos;
}

int display_y()
{
    return _y_pos;
}

int display_lineheight()
{
    return _line_height;
}

int display_fontwidth()
{
    return _font_width;
}