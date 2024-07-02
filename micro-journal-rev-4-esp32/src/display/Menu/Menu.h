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

#define MENU_HOME 0
#define MENU_SYNC 1
#define MENU_CLEAR 2
#define MENU_LAYOUT 3
#define MENU_WIFI 4
#define MENU_DRIVE 5
#define MENU_BLUETOOTH 6
#define MENU_BUTTONS 7
#define MENU_BACKGROUND 8
#define MENU_FOREGROUND 9
#define MENU_STARTUP 10


#endif