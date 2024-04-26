#ifndef Drive_h
#define Drive_h

#include <TFT_eSPI.h>
#include <U8g2_for_TFT_eSPI.h>

//
void Drive_setup(TFT_eSPI* ptft, U8g2_for_TFT_eSPI *pu8f);

// 
void Drive_render(TFT_eSPI* ptft, U8g2_for_TFT_eSPI *pu8f);

// 
void Drive_keyboard(char key);

#endif