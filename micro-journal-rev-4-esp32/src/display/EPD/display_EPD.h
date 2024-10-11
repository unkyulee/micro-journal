#ifndef Display_EPD_h
#define Display_EPD_h

#include <Arduino.h>
#include "epd_driver.h"

//
void display_EPD_setup();
void display_EPD_loop();
uint8_t *display_EPD_framebuffer();
void display_EPD_keyboard(char key);

#endif