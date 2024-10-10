#ifndef Display_ILI9341_h
#define Display_ILI9341_h

// TFT_eSPI setup
#include <SPI.h>
#include <TFT_eSPI.h>
#include <U8g2_for_TFT_eSPI.h>

//
void display_ILI9341_setup();
TFT_eSprite& display_ILI9341_sprite();

///
void display_ILI9341_loop();

#endif