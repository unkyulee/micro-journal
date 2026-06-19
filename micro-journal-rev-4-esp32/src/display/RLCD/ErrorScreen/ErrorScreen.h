#pragma once

#include "ST7305_4p2_BW_DisplayDriver.h"
#include "U8g2_for_ST73XX.h"

//
void ErrorScreen_setup(ST7305_4p2_BW_DisplayDriver *display, U8G2_FOR_ST73XX *u8);

//
void ErrorScreen_render(ST7305_4p2_BW_DisplayDriver *display, U8G2_FOR_ST73XX *u8);

// Whether anything visible changed during the last ErrorScreen_render() call, so the
// caller knows whether pushing the frame buffer over SPI is worth doing.
bool ErrorScreen_needsDisplay();

//
void ErrorScreen_keyboard(char key);
