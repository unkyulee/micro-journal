#ifndef ASCII_h
#define ASCII_h

#include <Arduino.h>

uint8_t ascii_international(uint8_t precursor, uint8_t ascii);
String ascii_convert_unicode(u_int8_t ascii);
uint8_t unicode_convert_ascii(String key);

#endif
