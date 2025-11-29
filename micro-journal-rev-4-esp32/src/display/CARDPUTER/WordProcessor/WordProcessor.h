#pragma once

#include <Arduino.h>

//
void WP_setup();

// 
void WP_render();

//
void WP_check_saved();
void WP_check_sleep();

//
void WP_render_clear();

//
void WP_render_status();

//
void WP_render_text();
void WP_render_line();

//
void WP_render_cursor();

// 
void WP_keyboard(int key, bool pressed);

//
String formatNumberWithCommas(long num);

//
String asciiToUnicode(uint8_t value);

