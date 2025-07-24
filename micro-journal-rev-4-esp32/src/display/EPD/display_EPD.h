#pragma once

#include <Arduino.h>
#include "epd_driver.h"
#include "fonts/monospace.h"

#include "fonts/system.h"


//
void display_EPD_setup();
void display_EPD_loop();
int display_EPD_core();

//
void display_EPD_keyboard(char key, bool pressed, int index);
void display_EPD_keyboard_report(uint8_t modifier, uint8_t reserved, uint8_t* keycodes);

//
GFXfont* display_EPD_font();

//
uint8_t *display_EPD_framebuffer();
void display_draw_buffer();

//
Rect_t display_rect(int x, int y, int width, int height);

//
void display_initialize(int x_init, int y_init, int line_height, int font_width);
void display_newline();
void display_setline(int row);
int display_x();
int display_y();
int display_lineheight();
int display_fontwidth();
