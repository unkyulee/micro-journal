#pragma once

// TFT_eSPI setup
#include <SPI.h>
#include <TFT_eSPI.h>
#include <U8g2_for_TFT_eSPI.h>

// for double buffering
TFT_eSprite &display_ST7735_sprite();

//
void display_ST7735_setup();
void display_ST7735_loop();
void display_ST7735_keyboard(int key, bool pressed, int index);
int display_ST7735_core();

//
const int colors[] = {
    TFT_BLACK,
    TFT_NAVY,
    TFT_DARKGREEN,
    TFT_DARKCYAN,
    TFT_MAROON,
    TFT_PURPLE,
    TFT_OLIVE,
    TFT_LIGHTGREY,
    TFT_DARKGREY,
    TFT_BLUE,
    TFT_GREEN,
    TFT_CYAN,
    TFT_RED,
    TFT_MAGENTA,
    TFT_YELLOW,
    TFT_WHITE,
    TFT_ORANGE,
    TFT_GREENYELLOW,
    TFT_PINK,
    TFT_BROWN,
    TFT_GOLD,
    TFT_SILVER,
    TFT_SKYBLUE,
    TFT_VIOLET};
