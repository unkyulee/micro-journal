#ifndef keyboard_locale_h
#define keyboard_locale_h

#include <Arduino.h>

// Returns the key value for the given HID keycode under the configured
// layout. int, not uint8_t: the Korean layout produces jamo unicode
// codepoints (0x3131-0x3163) that don't fit in a byte.
int keyboard_keycode_ascii(String locale, uint8_t keycode, bool shift, bool alt, bool pressed);

//
uint8_t keyboard_precursor_filter(uint8_t ascii);
uint8_t keyboard_caplock_filter(uint8_t ascii);

// translate internation key inputs ` + a = à 
uint8_t keyboard_international(uint8_t precursor, uint8_t ascii);

#endif
