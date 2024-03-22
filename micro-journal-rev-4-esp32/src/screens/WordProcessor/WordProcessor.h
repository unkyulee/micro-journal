#ifndef WORDPROCESSOR_h
#define WORDPROCESSOR_h

#include <TFT_eSPI.h>
#define FILENAME "/ujournal.txt"

//
void WP_setup(TFT_eSPI* ptft);

// 
void WP_render(TFT_eSPI* ptft);

// 
void WP_keyboard(char key);

//
void WP_load_text();
void WP_save_text();
void WP_empty_file();

//
void clear_background(TFT_eSPI *ptft);
void clear_trails(TFT_eSPI *ptft);
void blink_carrot(TFT_eSPI *ptft);
void check_saved();
String formatNumberWithCommas(long num);

#endif