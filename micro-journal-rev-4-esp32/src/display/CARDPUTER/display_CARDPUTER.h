#pragma once

//
#include "M5Cardputer.h"


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


//
void display_CARDPUTER_setup();
void display_CARDPUTER_loop();
int display_CARDPUTER_core();

//
void display_CARDPUTER_keyboard(int key, bool pressed, int index = -1);
void drawWrappedText(const String &text, int x, int y, int maxWidth);