#pragma once

#include <TFT_eSPI.h>
#include <U8g2_for_TFT_eSPI.h>

//
void Storage_setup(TFT_eSPI* ptft, U8g2_for_TFT_eSPI *pu8f);

// 
void Storage_render(TFT_eSPI* ptft, U8g2_for_TFT_eSPI *pu8f);

// 
void Storage_keyboard(char key, bool pressed);

