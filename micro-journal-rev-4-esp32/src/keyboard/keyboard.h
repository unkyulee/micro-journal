#pragma once

#include <Arduino.h>

void keyboard_setup();
void keyboard_loop();

bool keyboard_capslock();
void keyboard_capslock_toggle();

//
void keyboard_config_load(
    String filename,
    int *layers,
    int size,
    const char *keys[],
    int keyCount);
int keyboard_convert_HID(String _hid);
void keyboard_HID2Ascii(uint8_t keycode, uint8_t modifier, bool pressed);