#pragma once

#include <TFT_eSPI.h>
#include <U8g2_for_TFT_eSPI.h>

void Sleep_setup(TFT_eSPI* ptft, U8g2_for_TFT_eSPI *pu8f);
void Sleep_render(TFT_eSPI* ptft, U8g2_for_TFT_eSPI *pu8f);
void Sleep_keyboard(int key, bool pressed, int index);