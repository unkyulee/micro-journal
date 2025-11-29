#pragma once

//
#include "M5Cardputer.h"

//
void display_CARDPUTER_setup();
void display_CARDPUTER_loop();
int display_CARDPUTER_core();

//
M5Canvas* get_canvas();

//
void display_CARDPUTER_keyboard(int key, bool pressed, int index = -1);
