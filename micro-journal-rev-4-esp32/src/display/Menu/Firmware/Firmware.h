#ifndef Firmware_h
#define Firmware_h

#include <TFT_eSPI.h>
#include <U8g2_for_TFT_eSPI.h>

//
void Firmware_setup(TFT_eSPI* ptft, U8g2_for_TFT_eSPI *pu8f);

// 
void Firmware_render(TFT_eSPI* ptft, U8g2_for_TFT_eSPI *pu8f);

// 
void Firmware_keyboard(char key);

#endif