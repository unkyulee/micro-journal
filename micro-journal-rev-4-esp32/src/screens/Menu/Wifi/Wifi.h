#ifndef Wifi_h
#define Wifi_h

#include <TFT_eSPI.h>
#include <U8g2_for_TFT_eSPI.h>

//
void Wifi_setup(TFT_eSPI* ptft, U8g2_for_TFT_eSPI *pu8f);

// 
void Wifi_render(TFT_eSPI* ptft, U8g2_for_TFT_eSPI *pu8f);

// 
void Wifi_keyboard(char key);

#endif