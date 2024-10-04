#ifndef keyboard_locale_h
#define keyboard_locale_h

#include <Arduino.h>

uint8_t keyboard_keycode_ascii(String locale, uint8_t keycode, bool shift, bool alt, bool capslock);

#endif
