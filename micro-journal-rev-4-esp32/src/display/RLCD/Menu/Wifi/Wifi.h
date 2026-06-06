#pragma once

#include "ST7305_4p2_BW_DisplayDriver.h"
#include "U8g2_for_ST73XX.h"

//
void Wifi_setup(ST7305_4p2_BW_DisplayDriver *display, U8G2_FOR_ST73XX *u8);

// 
void Wifi_render(ST7305_4p2_BW_DisplayDriver *display, U8G2_FOR_ST73XX *u8);
void Wifi_render_list(ST7305_4p2_BW_DisplayDriver *display, U8G2_FOR_ST73XX *u8);
void Wifi_render_edit(ST7305_4p2_BW_DisplayDriver *display, U8G2_FOR_ST73XX *u8);

// 
void Wifi_keyboard(char key);

