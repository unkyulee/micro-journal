#include "display_CARDPUTER.h"
#include "../display.h"
#include "app/app.h"

//
#include "M5Cardputer.h"
#include "M5GFX.h"

//
#include "WordProcessor/WordProcessor.h"

M5Canvas canvas(&M5Cardputer.Display);
M5Canvas *get_canvas()
{
  return &canvas;
}

int display_CARDPUTER_core()
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
  return 0;
}

//
void display_CARDPUTER_setup()
{
  _log("DISPLAY CARDPUTER SETUP\n");
  M5Cardputer.begin();
  M5Cardputer.Display.setRotation(1);
  M5Cardputer.Display.setTextSize(0.5);
  M5Cardputer.Display.setTextFont(&fonts::FreeSerifBoldItalic18pt7b);
}

//
void display_CARDPUTER_loop()
{
  static unsigned int last = millis();
  if (millis() - last > 150)
  {
    last = millis();

    JsonDocument &app = status();
    int screen = app["screen"].as<int>();
    int screen_prev = app["screen_prev"].as<int>();

    // Word Processor
    // setup only once
    if (screen != screen_prev)
      WP_setup();
    else
      // loop
      WP_render();

    //
    app["screen_prev"] = screen;
  }
}

// Redirect the key press to the current GUI
void display_CARDPUTER_keyboard(int key, bool pressed, int index)
{
  JsonDocument &app = status();
  int screen = app["screen"].as<int>();
}

void display_CARDPUTER_keyboard_report(uint8_t modifier, uint8_t reserved, uint8_t *keycodes)
{
  JsonDocument &app = status();
  int screen = app["screen"].as<int>();
}