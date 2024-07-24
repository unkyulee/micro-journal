#ifndef Display_h
#define Display_h

// TFT_eSPI setup
#include <SPI.h>
#include <TFT_eSPI.h>
#include <U8g2_for_TFT_eSPI.h>

// Screen Type
#define WORDPROCESSOR 0
#define ERRORSCREEN 1
#define MENUSCREEN 2
#define WAKEUPSCREEN 3
#define SLEEPSCREEN 4
#define USBDRIVESCREEN 5

//
void display_setup();
TFT_eSprite& display_sprite();

///
void display_loop();

#endif