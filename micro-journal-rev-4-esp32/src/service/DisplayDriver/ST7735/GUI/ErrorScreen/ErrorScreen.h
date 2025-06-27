#ifndef ErrorScreen_h
#define ErrorScreen_h

#include <TFT_eSPI.h>
#include <U8g2_for_TFT_eSPI.h>

//
void ErrorScreen_setup(TFT_eSPI* ptft, U8g2_for_TFT_eSPI *pu8f);

// 
void ErrorScreen_render(TFT_eSPI* ptft, U8g2_for_TFT_eSPI *pu8f);

// 
void ErrorScreen_keyboard(char key);

#endif