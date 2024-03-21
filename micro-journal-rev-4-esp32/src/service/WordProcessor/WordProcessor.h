#ifndef WORDPROCESSOR_h
#define WORDPROCESSOR_h

#include <FS.h>
#include <TFT_eSPI.h>

//
void WP_setup();

///
void WP_loop();

//
void WP_load_text();
void WP_save_text();

// 
void WP_render(TFT_eSPI* tft);

// 
void WP_keyboard(char key);

#endif