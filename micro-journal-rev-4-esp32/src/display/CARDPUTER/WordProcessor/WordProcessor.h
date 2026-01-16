#pragma once

#include <Arduino.h>

//
void WP_setup();

// 
void WP_render();

//
void WP_check_saved();

//
void WP_render_clear();

//
void WP_render_status();

//
void WP_render_text();
void WP_render_line(int line_num, int y);

//
void WP_render_cursor();

// 
void WP_keyboard(int key, bool pressed, int index);


