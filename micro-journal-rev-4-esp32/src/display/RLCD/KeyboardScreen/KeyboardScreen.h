#pragma once

#include "ST7305_4p2_BW_DisplayDriver.h"
#include "U8g2_for_ST73XX.h"

void KeyboardScreen_setup(ST7305_4p2_BW_DisplayDriver *display, U8G2_FOR_ST73XX *u8);
void KeyboardScreen_render(ST7305_4p2_BW_DisplayDriver *display, U8G2_FOR_ST73XX *u8);

// Whether anything visible changed during the last KeyboardScreen_render() call, so the
// caller knows whether pushing the frame buffer over SPI is worth doing.
bool KeyboardScreen_needsDisplay();

void KeyboardScreen_keyboard(int key, bool pressed, int index = -1);

//
void KeyboardScreen_play(String macro);