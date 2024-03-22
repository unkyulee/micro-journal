#ifndef Menu_h
#define Menu_h

#include <TFT_eSPI.h>

//
void Menu_setup();

// 
void Menu_render(TFT_eSPI* ptft);

// 
void Menu_keyboard(char key);

#endif