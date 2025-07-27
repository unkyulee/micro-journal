#pragma once

#include <TFT_eSPI.h>
#include <U8g2_for_TFT_eSPI.h>

//
void PairBLE_setup(TFT_eSPI* ptft, U8g2_for_TFT_eSPI *pu8f);

// 
void PairBLE_render(TFT_eSPI* ptft, U8g2_for_TFT_eSPI *pu8f);
void PairBLE_render_list(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f);

// 
void PairBLE_keyboard(char key);

void PairBLE_load();
void PairBLE_save();

