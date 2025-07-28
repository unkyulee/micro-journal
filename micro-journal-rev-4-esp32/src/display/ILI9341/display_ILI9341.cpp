#include "display_ILI9341.h"
#include "../display.h"
#include "app/app.h"

// screens
#include "ErrorScreen/ErrorScreen.h"
#include "WakeUp/WakeUp.h"
#include "WordProcessor/WordProcessor.h"
#include "Sleep/Sleep.h"
#include "Menu/Menu.h"
#include "Update/Update.h"
#include "KeyboardScreen/KeyboardScreen.h"

// Invoke library, pins defined in platformio.ini
TFT_eSPI tft = TFT_eSPI();
TFT_eSprite sprite(&tft);
U8g2_for_TFT_eSPI u8f; // U8g2 font instance

//
TFT_eSprite &display_ILI9341_sprite()
{
  return sprite;
}

int display_ILI9341_core()
{
  JsonDocument &app = status();
  int screen = app["screen"].as<int>();

  // if app is not ready then run in the main core
  if (!app_ready())
    return 0;

  // IN MENU run it in the main core, so that sync process runs independently
  if (screen == MENUSCREEN)
  {
    int menu = app["menu"]["state"].as<int>();
    if (menu == MENU_SYNC)
      return 0;
  }

  // If BLE is set then let the BLE work on background alone
  if (app["config"]["ble"]["address"].is<const char *>())
  {
    return 0;
  }

  // by default run at the second core separated from keyboard loop
  return 1;
}

//
void display_ILI9341_setup()
{
  _log("DISPLAY ILI9341 SETUP\n");

  // Initialise the TFT screen
  tft.begin();

  // connect u8g2 procedures to TFT_eSPI
  u8f.begin(tft);

  //
  tft.setRotation(3);

  // Fill screen with grey so we can see the effect of printing with and without
  // a background colour defined
  tft.fillScreen(TFT_BLACK);
}

//
void display_ILI9341_loop()
{
  static unsigned int last = millis();
  if (millis() - last > 150)
  {
    last = millis();

    JsonDocument &app = status();
    int screen = app["screen"].as<int>();
    int screen_prev = app["screen_prev"].as<int>();

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

    // WAKEUP SCREEN
    else if (screen == WAKEUPSCREEN)
    {
      // setup only once
      if (screen != screen_prev)
        WakeUp_setup(&tft, &u8f);
      else
        // loop
        WakeUp_render(&tft, &u8f);
    }

    // WORD PROCESSOR
    else if (screen == WORDPROCESSOR)
    {
      // setup only once
      if (screen != screen_prev)
        WP_setup(&tft, &u8f);
      else
        // loop
        WP_render(&tft, &u8f);
    }

    // Keyboard Screen
    else if (screen == KEYBOARDSCREEN)
    {
      // setup only once
      if (screen != screen_prev)
        KeyboardScreen_setup(&tft, &u8f);
      else
        // loop
        KeyboardScreen_render(&tft, &u8f);
    }

    // SLEEP SCREEN
    else if (screen == SLEEPSCREEN)
    {
      // setup only once
      if (screen != screen_prev)
        Sleep_setup(&tft, &u8f);
      else
        // loop
        Sleep_render(&tft, &u8f);
    }

    // MENU SCREEN
    else if (screen == MENUSCREEN)
    {
      // setup only once
      if (screen != screen_prev)
        Menu_setup(&tft, &u8f);
      else
        // loop
        Menu_render(&tft, &u8f);
    }

    // UPDATE SCREEN
    else if (screen == UPDATESCREEN)
    {
      // setup only once
      if (screen != screen_prev)
        Update_setup(&tft, &u8f);
      else
        // loop
        Update_render(&tft, &u8f);
    }

    //
    app["screen_prev"] = screen;
  }
}

// Redirect the key press to the current GUI
void display_ILI9341_keyboard(char key, bool pressed, int index)
{
  JsonDocument &app = status();
  int screen = app["screen"].as<int>();

  if (screen == ERRORSCREEN)
  {
    if (!pressed)
      ErrorScreen_keyboard(key);
  }

  else if (screen == WAKEUPSCREEN)
  {
    if (!pressed)
      WakeUp_keyboard(key, pressed, index);
  }

  else if (screen == WORDPROCESSOR)
  {
    // send the key stroke to word processor
    WP_keyboard(key, pressed, index);
  }

  if (screen == KEYBOARDSCREEN)
  {
    // send the key stroke to word processor
    KeyboardScreen_keyboard(key, pressed, index);
  }

  else if (screen == SLEEPSCREEN)
  {
    if (!pressed)
      Sleep_keyboard(key, pressed, index);
  }
  else if (screen == MENUSCREEN)
  {
    if (!pressed)
      Menu_keyboard(key);
  }
  else if (screen == UPDATESCREEN)
  {
    if (!pressed)
      Update_keyboard(key);
  }
}

void display_ILI9341_keyboard_report(uint8_t modifier, uint8_t reserved, uint8_t *keycodes)
{
  JsonDocument &app = status();
  int screen = app["screen"].as<int>();
}