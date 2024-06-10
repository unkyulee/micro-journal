#ifndef USBDRIVE_h
#define USBDRIVE_h

#include <TFT_eSPI.h>
#include <U8g2_for_TFT_eSPI.h>

//
void USBDRIVE_setup(TFT_eSPI* ptft, U8g2_for_TFT_eSPI *pu8f);

// 
void USBDRIVE_render(TFT_eSPI* ptft, U8g2_for_TFT_eSPI *pu8f);

// 
void USBDRIVE_keyboard(char key);

#endif