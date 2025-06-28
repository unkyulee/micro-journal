#ifndef Display_ST7735_h
#define Display_ST7735_h

// TFT_eSPI setup
#include <SPI.h>
#include <TFT_eSPI.h>
#include <U8g2_for_TFT_eSPI.h>

// for double buffering
TFT_eSprite& display_ST7735_sprite();

//
void display_ST7735_setup();
void display_ST7735_loop();
void display_ST7735_keyboard(int key, bool pressed);

#endif