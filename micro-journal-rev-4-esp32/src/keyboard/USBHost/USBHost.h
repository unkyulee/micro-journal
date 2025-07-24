#pragma once

#include <Arduino.h>

void USBHost_setup();
void USBHost_loop();

//
void USBHost_keyboard(uint8_t keycode, uint8_t modifier, bool pressed);
void USBHost_report(uint8_t modifier, uint8_t reserved, uint8_t* keycodes);
