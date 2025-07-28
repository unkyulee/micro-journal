#pragma once

#include <TFT_eSPI.h>
#include <U8g2_for_TFT_eSPI.h>

//
void Info_setup(TFT_eSPI* ptft, U8g2_for_TFT_eSPI *pu8f);

// 
void Info_render(TFT_eSPI* ptft, U8g2_for_TFT_eSPI *pu8f);

// 
void Info_keyboard(char key, bool pressed);