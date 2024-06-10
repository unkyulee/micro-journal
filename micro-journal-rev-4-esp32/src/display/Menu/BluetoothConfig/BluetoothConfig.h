#ifndef Bluetooth_Config_h
#define Bluetooth_Config_h

#include <TFT_eSPI.h>
#include <U8g2_for_TFT_eSPI.h>

//
void BluetoothConfig_setup(TFT_eSPI* ptft, U8g2_for_TFT_eSPI *pu8f);

// 
void BluetoothConfig_render(TFT_eSPI* ptft, U8g2_for_TFT_eSPI *pu8f);

// 
void BluetoothConfig_keyboard(char key);

#endif