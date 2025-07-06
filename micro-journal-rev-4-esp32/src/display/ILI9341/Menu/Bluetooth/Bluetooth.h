#pragma once

#ifdef BLE

#include <TFT_eSPI.h>
#include <U8g2_for_TFT_eSPI.h>

//
void Bluetooth_setup(TFT_eSPI* ptft, U8g2_for_TFT_eSPI *pu8f);

// 
void Bluetooth_render(TFT_eSPI* ptft, U8g2_for_TFT_eSPI *pu8f);

// 
void Bluetooth_keyboard(char key);

void Bluetooth_load();
void Bluetooth_save();

#endif