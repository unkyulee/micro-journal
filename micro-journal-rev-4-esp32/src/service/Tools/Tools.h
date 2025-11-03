#pragma once

#include <Arduino.h>

String formatNumber(int num);
size_t fileSize(String fileName);
String asciiToUnicode(uint8_t value);
String format(const char *format, ...);


#if defined(DEBUG) && defined(BOARD_PICO)
void printMemoryUsage();
#endif
