#ifndef Menu_h
#define Menu_h

#include <TFT_eSPI.h>
#include <U8g2_for_TFT_eSPI.h>

//
void Menu_setup(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f);

//
void Menu_render(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f);

//
void Menu_keyboard(char key);

//
void Menu_clear();

//
void Menu_sync(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f);


#endif