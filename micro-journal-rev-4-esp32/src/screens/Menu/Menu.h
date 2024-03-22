#ifndef Menu_h
#define Menu_h

#include <TFT_eSPI.h>

//
void Menu_setup(TFT_eSPI* ptft);

// 
void Menu_render(TFT_eSPI* ptft);

// 
void Menu_keyboard(char key);

//
void Menu_sync(TFT_eSPI* ptft);

#endif