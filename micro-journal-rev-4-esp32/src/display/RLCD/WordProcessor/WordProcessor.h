#pragma once

#include "ST7305_4p2_BW_DisplayDriver.h"
#include "U8g2_for_ST73XX.h"
//
void WP_setup(ST7305_4p2_BW_DisplayDriver *display, U8G2_FOR_ST73XX *u8);

//
void WP_render(ST7305_4p2_BW_DisplayDriver *display, U8G2_FOR_ST73XX *u8);

// Whether anything visible changed during the last WP_render() call, so the
// caller knows whether pushing the frame buffer over SPI is worth doing.
bool WP_needsDisplay();

//
void WP_check_saved();

//
void WP_render_clear(ST7305_4p2_BW_DisplayDriver *display, U8G2_FOR_ST73XX *u8);

//
void WP_render_status(ST7305_4p2_BW_DisplayDriver *display, U8G2_FOR_ST73XX *u8);

//
void WP_render_text(ST7305_4p2_BW_DisplayDriver *display, U8G2_FOR_ST73XX *u8);
void WP_render_line(ST7305_4p2_BW_DisplayDriver *display, U8G2_FOR_ST73XX *u8, int line_num);

//
void WP_render_cursor(ST7305_4p2_BW_DisplayDriver *display, U8G2_FOR_ST73XX *u8);

//

// 
void WP_keyboard(int key, bool pressed, int index = -1);

//
String formatNumberWithCommas(long num);

