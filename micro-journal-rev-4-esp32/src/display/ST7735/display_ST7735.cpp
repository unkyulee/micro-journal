#include "display_ST7735.h"
#include "../display.h"
#include "app/app.h"

// screens
/*
#include "GUI/WordProcessor/WordProcessor.h"
#include "GUI/ErrorScreen/ErrorScreen.h"
#include "GUI/Menu/Menu.h"
#include "GUI/WakeUp/WakeUp.h"
*/

// Invoke library, pins defined in platformio.ini
TFT_eSPI tft = TFT_eSPI();
TFT_eSprite sprite(&tft);
U8g2_for_TFT_eSPI u8f; // U8g2 font instance

//
TFT_eSprite &display_ST7735_sprite()
{
  return sprite;
}

//
void display_ST7735_setup()
{
}

//
void display_ST7735_loop()
{
}

// Redirect the key press to the current GUI
void display_ST7735_keyboard(char key)
{
}