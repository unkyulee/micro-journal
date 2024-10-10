#ifndef Foreground_h
#define Foreground_h

#include <TFT_eSPI.h>
#include <U8g2_for_TFT_eSPI.h>

//
void Foreground_setup(TFT_eSPI* ptft, U8g2_for_TFT_eSPI *pu8f);

// 
void Foreground_render(TFT_eSPI* ptft, U8g2_for_TFT_eSPI *pu8f);

// 
void Foreground_keyboard(char key);

#endif