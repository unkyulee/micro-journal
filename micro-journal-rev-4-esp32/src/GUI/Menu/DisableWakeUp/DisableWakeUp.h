#ifndef DisableWakeUp_h
#define DisableWakeUp_h

#include <TFT_eSPI.h>
#include <U8g2_for_TFT_eSPI.h>

//
void DisableWakeUp_setup(TFT_eSPI* ptft, U8g2_for_TFT_eSPI *pu8f);

// 
void DisableWakeUp_render(TFT_eSPI* ptft, U8g2_for_TFT_eSPI *pu8f);

// 
void DisableWakeUp_keyboard(char key);

#endif