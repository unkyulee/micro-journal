#ifndef Display_EPD_h
#define Display_EPD_h

#include <Arduino.h>
#include "epd_driver.h"
#include "fonts/monospace.h"

#include "fonts/system.h"


//
void display_EPD_setup();
void display_EPD_loop();
void display_EPD_keyboard(char key);

//
uint8_t *display_EPD_framebuffer();

//
GFXfont* display_EPD_font();

//
Rect_t display_rect(int x, int y, int width, int height);

#endif