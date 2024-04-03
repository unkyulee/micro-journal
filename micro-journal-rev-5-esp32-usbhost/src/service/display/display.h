#ifndef Display_h
#define Display_h

// TFT_eSPI setup
#include <TFT_eSPI.h>
#include <SPI.h>

// Screen Type
#define WORDPROCESSOR 0
#define ERRORSCREEN 1
#define MENUSCREEN 2

//
void display_setup();

///
void display_loop();

#endif