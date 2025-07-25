#pragma once

#include <TFT_eSPI.h>
#include <U8g2_for_TFT_eSPI.h>

//
void Send_setup(TFT_eSPI* ptft, U8g2_for_TFT_eSPI *pu8f);

// 
void Send_render(TFT_eSPI* ptft, U8g2_for_TFT_eSPI *pu8f);

// 
void Send_keyboard(char key, bool pressed);

