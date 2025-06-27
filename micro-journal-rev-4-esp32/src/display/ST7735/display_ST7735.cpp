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
  //
  app_log("Display ST7735 setup called\n");

  // Initialize the TFT display
  tft.init();
  tft.setRotation(1);        // Set rotation to landscape mode
  tft.fillScreen(TFT_WHITE); // Clear the screen with black color

  // Initialize the sprite
  sprite.setColorDepth(8);
  sprite.createSprite(80, 160); // Create a sprite with the same size as the display
  sprite.fillSprite(TFT_BLACK); // Fill the sprite with black color

  // Initialize U8g2 for text rendering
  u8f.begin(tft);
  u8f.setFont(u8g2_font_profont22_tf); // Set a font for U8g2
}

//
void display_ST7735_loop()
{
  static unsigned int last = millis();
  if (millis() - last > 1500)
  {
    last = millis();

    ///
    tft.setCursor(0, 0, 2);
    tft.setTextSize(1); // Adjust text size as needed
    tft.setTextColor(TFT_WHITE, TFT_RED);

    // Draw text in the middle of the screen
    tft.setCursor(0, 0, 2);
    tft.print("asdfasdf");

    //
    app_log("Display ST7735 loop called\n");
  }
}

// Redirect the key press to the current GUI
void display_ST7735_keyboard(char key)
{
}