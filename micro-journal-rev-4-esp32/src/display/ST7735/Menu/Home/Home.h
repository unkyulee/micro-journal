#pragma once

#include <TFT_eSPI.h>
#include <U8g2_for_TFT_eSPI.h>

//
void Home_setup(TFT_eSPI* ptft, U8g2_for_TFT_eSPI *pu8f);

// 
void Home_render(TFT_eSPI* ptft, U8g2_for_TFT_eSPI *pu8f);

// 
void Home_keyboard(char key, bool pressed);
