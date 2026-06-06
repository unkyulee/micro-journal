#pragma once

// TFT_eSPI setup
#include <SPI.h>


//
void display_RLCD_setup();
void display_RLCD_loop();
int display_RLCD_core();

//
void display_RLCD_keyboard(int key, bool pressed, int index = -1);
