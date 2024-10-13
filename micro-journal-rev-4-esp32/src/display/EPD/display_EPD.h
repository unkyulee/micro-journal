#ifndef Display_EPD_h
#define Display_EPD_h

#include <Arduino.h>
#include "epd_driver.h"
#include "fonts/monospace.h"
#include "fonts/system.h"

//
void display_EPD_setup();
void display_EPD_loop();
uint8_t *display_EPD_framebuffer();
void display_EPD_keyboard(char key);

//
GFXfont* display_EPD_font();

#endif