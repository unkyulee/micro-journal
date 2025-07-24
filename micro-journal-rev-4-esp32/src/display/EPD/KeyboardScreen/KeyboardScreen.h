#pragma once

#include <Arduino.h>

void KeyboardScreen_setup();
void KeyboardScreen_render();
void KeyboardScreen_keyboard(uint8_t modifier, uint8_t reserved, uint8_t* keycodes);
