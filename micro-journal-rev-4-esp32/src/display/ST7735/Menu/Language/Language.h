#pragma once

#include <TFT_eSPI.h>
#include <U8g2_for_TFT_eSPI.h>

// Keyboard language selection: English (US) or Korean (KR). Selecting a
// language also switches the word processor font (index into WP_FONTS in
// WordProcessor.cpp), since Korean needs the Hangul font.
void Language_setup(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f);
void Language_render(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f);
void Language_keyboard(char key, bool pressed);
