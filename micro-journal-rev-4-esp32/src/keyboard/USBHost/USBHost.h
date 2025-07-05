#pragma once

#include <Arduino.h>

void USBHost_setup();
void USBHost_loop();

//
void USBHost_keyboard(uint8_t keycode, uint8_t modifier, bool pressed);
