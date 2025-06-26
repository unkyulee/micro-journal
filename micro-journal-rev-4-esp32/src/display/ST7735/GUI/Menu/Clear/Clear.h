#ifndef Clear_h
#define Clear_h

#include <TFT_eSPI.h>
#include <U8g2_for_TFT_eSPI.h>

//
void Clear_setup(TFT_eSPI* ptft, U8g2_for_TFT_eSPI *pu8f);

// 
void Clear_render(TFT_eSPI* ptft, U8g2_for_TFT_eSPI *pu8f);

// 
void Clear_keyboard(char key);

#endif