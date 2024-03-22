#ifndef WORDPROCESSOR_h
#define WORDPROCESSOR_h

#include <TFT_eSPI.h>

//
void WP_setup();

// 
void WP_render(TFT_eSPI* ptft);

// 
void WP_keyboard(char key);

//
void WP_load_text();
void WP_save_text();

#endif