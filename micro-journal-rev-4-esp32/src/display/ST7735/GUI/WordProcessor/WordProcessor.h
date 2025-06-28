#pragma once

#include <TFT_eSPI.h>
#include <U8g2_for_TFT_eSPI.h>

//
void WP_setup(TFT_eSPI* ptft, U8g2_for_TFT_eSPI *pu8f);

// 
void WP_render(TFT_eSPI* ptft, U8g2_for_TFT_eSPI *pu8f);

//
void WP_check_saved();
void WP_check_sleep();

//
void WP_render_clear(TFT_eSPI* ptft, U8g2_for_TFT_eSPI *pu8f);

//
void WP_render_status(TFT_eSPI* ptft, U8g2_for_TFT_eSPI *pu8f);

//
void WP_render_text(TFT_eSPI* ptft, U8g2_for_TFT_eSPI *pu8f);
void WP_render_line(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f, int line_num);

//
void WP_render_cursor(TFT_eSPI* ptft, U8g2_for_TFT_eSPI *pu8f);

// 
void WP_keyboard(int key, bool pressed);

//
String formatNumberWithCommas(long num);

//
String asciiToUnicode(uint8_t value);

