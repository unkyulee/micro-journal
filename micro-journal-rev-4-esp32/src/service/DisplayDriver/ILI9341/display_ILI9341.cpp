#include "display_ILI9341.h"
#include "../display.h"
#include "app/app.h"

// screens
#include "GUI/WordProcessor/WordProcessor.h"
#include "GUI/ErrorScreen/ErrorScreen.h"
#include "GUI/Menu/Menu.h"
#include "GUI/WakeUp/WakeUp.h"

// Invoke library, pins defined in platformio.ini
TFT_eSPI tft = TFT_eSPI();
TFT_eSprite sprite(&tft);
U8g2_for_TFT_eSPI u8f; // U8g2 font instance

//
TFT_eSprite& display_ILI9341_sprite() {
  return sprite;
}

//
void display_ILI9341_setup()
{
  app_log("DISPLAY ILI9341 SETUP\n");

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

    JsonDocument &app = app_status();
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

    // WAKEUP SCREEN
    else if (screen == WAKEUPSCREEN)
    {
      // setup only once
      if (screen != screen_prev)
        WakeUp_setup(&tft, &u8f, true);
      else
        // loop
        WakeUp_render(&tft, &u8f);
    }

    // SLEEP SCREEN
    else if (screen == SLEEPSCREEN)
    {
      // setup only once
      if (screen != screen_prev)
        WakeUp_setup(&tft, &u8f, false);
      else
        // loop
        WakeUp_render(&tft, &u8f);
    }

    // WORD PROCESSOR
    else
    {
      // setup only once
      if (screen != screen_prev)
        WP_setup(&tft, &u8f);
      else
        // loop
        WP_render(&tft, &u8f);
    }

    //
    app["screen_prev"] = screen;
  }
}

// Redirect the key press to the current GUI
void display_ILI9341_keyboard(char key)
{
  JsonDocument &app = app_status();
  int screen = app["screen"].as<int>();

  if (screen == WORDPROCESSOR)
  {
    // send the key stroke to word processor
    WP_keyboard(key);
  }
  else if (screen == MENUSCREEN)
  {
    Menu_keyboard(key);
  }
  else if (screen == ERRORSCREEN)
  {
    ErrorScreen_keyboard(key);
  }
  else if (screen == WAKEUPSCREEN || screen == SLEEPSCREEN)
  {
    WakeUp_keyboard(key);
  }
}