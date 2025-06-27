#ifndef Reset_h
#define Reset_h

#include <TFT_eSPI.h>
#include <U8g2_for_TFT_eSPI.h>

//
void Reset_setup(TFT_eSPI* ptft, U8g2_for_TFT_eSPI *pu8f);

// 
void Reset_render(TFT_eSPI* ptft, U8g2_for_TFT_eSPI *pu8f);

// 
void Reset_keyboard(char key);

#endif