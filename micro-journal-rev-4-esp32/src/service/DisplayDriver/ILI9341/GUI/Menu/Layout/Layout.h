#ifndef Layout_h
#define Layout_h

#include <TFT_eSPI.h>
#include <U8g2_for_TFT_eSPI.h>

//
void Layout_setup(TFT_eSPI* ptft, U8g2_for_TFT_eSPI *pu8f);

// 
void Layout_render(TFT_eSPI* ptft, U8g2_for_TFT_eSPI *pu8f);

// 
void Layout_keyboard(char key);

#endif