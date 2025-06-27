#ifndef FrontPanelButton_h
#define FrontPanelButton_h

#include <TFT_eSPI.h>
#include <U8g2_for_TFT_eSPI.h>

//
void FrontPanelButton_setup(TFT_eSPI* ptft, U8g2_for_TFT_eSPI *pu8f);

// 
void FrontPanelButton_render(TFT_eSPI* ptft, U8g2_for_TFT_eSPI *pu8f);

// 
void FrontPanelButton_keyboard(char key);

#endif