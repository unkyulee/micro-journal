#include "display_RLCD.h"
#include "../display.h"
#include "app/app.h"

//
#include <SPI.h>
#include <ST73xxPins.h>
#include "U8g2_for_ST73XX.h"
#include "ST7305_4p2_BW_DisplayDriver.h"

// SETUP SPI for the DISPLAY
const ST73xxPins PINS{PIN_DC, PIN_CS, PIN_SCLK, PIN_MOSI, PIN_RST};
ST7305_4p2_BW_DisplayDriver display(PINS, SPI);
U8G2_FOR_ST73XX u8g2;

// screens
#include "ErrorScreen/ErrorScreen.h"
#include "WordProcessor/WordProcessor.h"
#include "Menu/Menu.h"
#include "Update/Update.h"
#include "KeyboardScreen/KeyboardScreen.h"

int display_RLCD_core()
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
void display_RLCD_setup()
{
  _log("DISPLAY RLCD SETUP\n");

  // SPI SETUP
  SPI.begin(PIN_SCLK, -1, PIN_MOSI, PIN_CS);

  // Initialise the screen
  display.initialize();
  display.High_Power_Mode();
  display.display_on(true);
  display.display_Inversion(false);
  display.clearDisplay();

  // connect u8g2 procedures to TFT_eSPI
  u8g2.begin(display);
  u8g2.setFontMode(0);
  u8g2.setForegroundColor(ST7305_COLOR_BLACK);
  u8g2.setBackgroundColor(ST7305_COLOR_WHITE);
}

//
void display_RLCD_loop()
{
  static unsigned int last = millis();
  if (millis() - last > 100)
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
        ErrorScreen_setup(&display, &u8g2);
      else
        // loop
        ErrorScreen_render(&display, &u8g2);
    }

    // SLEEP WAKEUP
    else if (screen == SLEEPSCREEN || screen == WAKEUPSCREEN)
    {
      // redirect to WORDPROCESSOR
      app["screen"] = WORDPROCESSOR;
    }

    // WORD PROCESSOR
    else if (screen == WORDPROCESSOR)
    {
      // setup only once
      if (screen != screen_prev)
        WP_setup(&display, &u8g2);
      else
        // loop
        WP_render(&display, &u8g2);
    }

    // Keyboard Screen
    else if (screen == KEYBOARDSCREEN)
    {
      // setup only once
      if (screen != screen_prev)
        KeyboardScreen_setup(&display, &u8g2);
      else
        // loop
        KeyboardScreen_render(&display, &u8g2);
    }

    // MENU SCREEN
    else if (screen == MENUSCREEN)
    {
      // setup only once
      if (screen != screen_prev)
        Menu_setup(&display, &u8g2);
      else
        // loop
        Menu_render(&display, &u8g2);
    }

    // UPDATE SCREEN
    else if (screen == UPDATESCREEN)
    {
      // setup only once
      if (screen != screen_prev)
        Update_setup(&display, &u8g2);
      else
        // loop
        Update_render(&display, &u8g2);
    }

    //
    app["screen_prev"] = screen;

    // Every screen tracks whether anything visible actually changed since
    // the last push, since display() always transfers the whole 30KB frame
    // buffer over SPI regardless of how much changed. The first tick after
    // switching screens (setup) always pushes once to show the initial frame.
    bool shouldDisplay = true;
    if (screen == screen_prev)
    {
      if (screen == ERRORSCREEN)
        shouldDisplay = ErrorScreen_needsDisplay();
      else if (screen == WORDPROCESSOR)
        shouldDisplay = WP_needsDisplay();
      else if (screen == KEYBOARDSCREEN)
        shouldDisplay = KeyboardScreen_needsDisplay();
      else if (screen == MENUSCREEN)
        shouldDisplay = Menu_needsDisplay();
      else if (screen == UPDATESCREEN)
        shouldDisplay = Update_needsDisplay();
    }

    if (shouldDisplay)
      display.display();
  }
}

// Redirect the key press to the current GUI
void display_RLCD_keyboard(int key, bool pressed, int index)
{
  JsonDocument &app = status();
  int screen = app["screen"].as<int>();

  if (screen == ERRORSCREEN)
  {
    if (!pressed)
      ErrorScreen_keyboard(key);
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

void display_RLCD_keyboard_report(uint8_t modifier, uint8_t reserved, uint8_t *keycodes)
{
  JsonDocument &app = status();
  int screen = app["screen"].as<int>();
}