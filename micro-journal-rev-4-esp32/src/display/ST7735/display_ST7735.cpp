#include "display_ST7735.h"
#include "display/display.h"
#include "app/app.h"

// screens
#include "GUI/ErrorScreen/ErrorScreen.h"
#include "GUI/WordProcessor/WordProcessor.h"
#include "GUI/Menu/Menu.h"
#include "GUI/Keyboard/Keyboard.h"

/*
#include "GUI/Menu/Menu.h"
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
  _log("Display ST7735 setup called\n");

  // Initialise the TFT screen
  tft.begin();

  //
  tft.setRotation(1);

  // Fill screen with grey so we can see the effect of printing with and without
  // a background colour defined
  tft.fillScreen(TFT_BLACK);

  // connect u8g2 procedures to TFT_eSPI
  u8f.begin(tft);
}

//
void display_ST7735_loop()
{
  static unsigned int last = millis();
  if (millis() - last > 150)
  {
    last = millis();

    JsonDocument &app = status();
    int screen = app["screen"].as<int>();
    int screen_prev = app["screen_prev"].as<int>();

    //_log("Display ST7735 loop called, screen: %d, screen_prev: %d\n", screen, screen_prev);

    // ERROR SCREEN
    if (screen == ERRORSCREEN)
    {
      // setup only once
      if (screen != screen_prev)
        ErrorScreen_setup(&tft, &u8f);
      else
        // loop
        ErrorScreen_render(&tft, &u8f);
    }

    // TBD: Wakeup Screen
    // Word Processor
    else if (screen == WAKEUPSCREEN || screen == WORDPROCESSOR)
    {
      // TBD: to be deleted: skip wakeupscreen
      screen = WORDPROCESSOR;
      app["screen"] = WORDPROCESSOR;

      // setup only once
      if (screen != screen_prev)
        WP_setup(&tft, &u8f);
      else
        // loop
        WP_render(&tft, &u8f);
    }

    // Menu Screen
    else if (screen == MENUSCREEN)
    {
      // setup only once
      if (screen != screen_prev)
        Menu_setup(&tft, &u8f);
      else
        // loop
        Menu_render(&tft, &u8f);
    }

    // KEYBOARDSCREEN
    else if (screen == KEYBOARDSCREEN)
    {
      // setup only once
      if (screen != screen_prev)
        KeyboardScreen_setup(&tft, &u8f);
      else
        // loop
        KeyboardScreen_render(&tft, &u8f);
    }

    //
    app["screen_prev"] = screen;
  }
}

// Redirect the key press to the current GUI
void display_ST7735_keyboard(int key, bool pressed, int index)
{
  //
  JsonDocument &app = status();
  int screen = app["screen"].as<int>();

  _debug("[display_ST7735_keyboard] Key: %c [%d][%d] Screen: %d\n", key, key, index, screen);

  if (screen == ERRORSCREEN)
  {
    ErrorScreen_keyboard(key);
  }
  else if (screen == WORDPROCESSOR)
  {
    // send the key stroke to word processor
    WP_keyboard(key, pressed);
  }
  else if (screen == MENUSCREEN)
  {
    // send the key stroke to word processor
    Menu_keyboard(key, pressed);
  }
  else if(screen == KEYBOARDSCREEN) {
    KeyboardScreen_keyboard(key, pressed, index);
  }
}