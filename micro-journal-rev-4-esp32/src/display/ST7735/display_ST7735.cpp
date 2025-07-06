#include "display_ST7735.h"
#include "display/display.h"
#include "app/app.h"
#include "service/Editor/Editor.h"

// screens
#include "ErrorScreen/ErrorScreen.h"
#include "WordProcessor/WordProcessor.h"
#include "Menu/Menu.h"
#include "KeyboardScreen/KeyboardScreen.h"
#include "WakeUp/WakeUp.h"
#include "Sleep/Sleep.h"

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

  // set brightness to 10 if config not set
  JsonDocument &app = status();
  if (!app["config"]["brightness"].is<int>())
  {
    app["config"]["brightness"] = 10;
  }

  //
  int brightness = app["config"]["brightness"].as<int>();
  pinMode(TFT_BL, OUTPUT);
  analogWrite(TFT_BL, 255 / (11 - brightness)); // Set brightness (0=off, 255=full)
}

//
void display_ST7735_loop()
{
  JsonDocument &app = status();
  int screen = app["screen"].as<int>();

  //
  static unsigned int last = millis();
  if (millis() - last > 150)
  {
    last = millis();

    int screen_prev = app["screen_prev"].as<int>();
    int brightness = app["config"]["brightness"].as<int>();
    analogWrite(TFT_BL, 255 / (11 - brightness)); // Set brightness (0=off, 255=full)

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

    // Wakeup Screen
    else if (screen == WAKEUPSCREEN)
    {
      // setup only once
      if (screen != screen_prev)
        WakeUp_setup(&tft, &u8f);
      else
        // loop
        WakeUp_render(&tft, &u8f);
    }

    // Sleep Screen
    else if (screen == SLEEPSCREEN)
    {
      // setup only once
      if (screen != screen_prev)
        Sleep_setup(&tft, &u8f);
      else
        // loop
        Sleep_render(&tft, &u8f);
    }

    // Word Processor
    else if (screen == WORDPROCESSOR)
    {
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
    if(!pressed) ErrorScreen_keyboard(key);
  }
  else if (screen == WORDPROCESSOR)
  {
    // send the key stroke to word processor
    WP_keyboard(key, pressed);
  }
  else if (screen == WAKEUPSCREEN)
  {
    // send the key stroke to wakeup screen
    if(!pressed) WakeUp_keyboard(key, pressed, index);
  }
  else if (screen == SLEEPSCREEN)
  {
    // send the key stroke to sleep screen
    if(!pressed) Sleep_keyboard(key, pressed, index);
  }
  else if (screen == MENUSCREEN)
  {
    // send the key stroke to word processor
    if(!pressed) Menu_keyboard(key, pressed);
  }
  else if (screen == KEYBOARDSCREEN)
  {
    KeyboardScreen_keyboard(key, pressed, index);
  }
}

int display_ST7735_core()
{
  // in case of using GIF or blocking task then move it to core 1
  JsonDocument &app = status();
  int screen = app["screen"].as<int>();

  if (screen == KEYBOARDSCREEN || screen == WAKEUPSCREEN || screen == SLEEPSCREEN)
  {
    return 1;
  }

  // by default run at the main core
  return 0;
}