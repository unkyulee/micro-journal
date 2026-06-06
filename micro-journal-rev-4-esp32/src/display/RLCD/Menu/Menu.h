#pragma once

#include "ST7305_4p2_BW_DisplayDriver.h"
#include "U8g2_for_ST73XX.h"

//
void Menu_setup(ST7305_4p2_BW_DisplayDriver *display, U8G2_FOR_ST73XX *u8);

//
void Menu_render(ST7305_4p2_BW_DisplayDriver *display, U8G2_FOR_ST73XX *u8);

//
void Menu_keyboard(char key);

//
void Menu_clear();

//
void Menu_sync(ST7305_4p2_BW_DisplayDriver *display, U8G2_FOR_ST73XX *u8);

