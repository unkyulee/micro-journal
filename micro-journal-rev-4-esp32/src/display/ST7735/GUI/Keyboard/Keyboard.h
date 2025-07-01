#pragma once

#include <TFT_eSPI.h>
#include <U8g2_for_TFT_eSPI.h>

void KeyboardScreen_setup(TFT_eSPI* ptft, U8g2_for_TFT_eSPI *pu8f);
void KeyboardScreen_render(TFT_eSPI* ptft, U8g2_for_TFT_eSPI *pu8f);
void KeyboardScreen_keyboard(int key, bool pressed, int index);