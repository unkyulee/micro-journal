#include "display_CARDPUTER.h"
#include "../display.h"
#include "app/app.h"

//
#include "M5Cardputer.h"
#include "M5GFX.h"

//
#include "ErrorScreen/ErrorScreen.h"
#include "WakeUp/WakeUp.h"
#include "WordProcessor/WordProcessor.h"
#include "Update/Update.h"
#include "Menu/Menu.h"

//
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
  return 1;
}

//
void display_CARDPUTER_setup()
{
  _log("DISPLAY CARDPUTER SETUP\n");

  M5Cardputer.Display.setRotation(1);
}

//
void display_CARDPUTER_loop()
{
  static unsigned int last = millis();
  if (millis() - last > 150)
  {
    last = millis();

    JsonDocument &app = status();

    // display brightness
    if (app["config"]["brightness"].is<int>())
    {
      int brightness = app["config"]["brightness"].as<int>();
      M5Cardputer.Display.setBrightness(min(25 * (brightness+1), 255));
    }
    

    //
    int screen = app["screen"].as<int>();
    int screen_prev = app["screen_prev"].as<int>();

    //
    if (screen != screen_prev)
    {
      _log("Loading Screen: %d\n", screen);
    }

    // Error Screen
    if (screen == ERRORSCREEN)
    {
      if (screen != screen_prev)
        ErrorScreen_setup();
      else
        // loop
        ErrorScreen_render();
    }

    // Wake up Screen
    else if (screen == WAKEUPSCREEN)
    {
      if (screen != screen_prev)
        WakeUp_setup();
      else
        // loop
        WakeUp_render();
    }

    // Word Processor
    else if (screen == WORDPROCESSOR)
    {
      if (screen != screen_prev)
        WP_setup();
      else
        // loop
        WP_render();
    }

    // MENU SCREEN
    else if (screen == MENUSCREEN)
    {
      // setup only once
      if (screen != screen_prev)
        Menu_setup();
      else
        // loop
        Menu_render();
    }

    // UPDATE SCREEN
    else if (screen == UPDATESCREEN)
    {
      // setup only once
      if (screen != screen_prev)
        Update_setup();
      else
        // loop
        Update_render();
    }

    //
    app["screen_prev"] = screen;
  }
}

// Redirect the key press to the current GUI
void display_CARDPUTER_keyboard(int key, bool pressed, int index)
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
      WakeUp_keyboard(key);
  }

  else if (screen == WORDPROCESSOR)
  {
    // send the key stroke to word processor
    WP_keyboard(key, pressed, index);
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

void drawWrappedText(const String &text, int x, int y, int maxWidth)
{
  int cursorY = y;
  int start = 0;

  while (start < text.length())
  {
    int end = start;
    int lastSpace = -1;
    int width = 0;

    // Find longest line that fits
    while (end < text.length())
    {
      char c = text[end];
      int charWidth = M5Cardputer.Display.textWidth(String(c));

      // Stop if this character exceeds width
      if (width + charWidth > maxWidth)
        break;

      width += charWidth;

      if (c == ' ')
        lastSpace = end;

      end++;
    }

    // If we overflow in the middle of a word
    if (end < text.length() && lastSpace >= start)
      end = lastSpace;

    // Extract line
    String line = text.substring(start, end);

    // Draw line
    M5Cardputer.Display.drawString(line, x, cursorY);

    // Move to next line
    cursorY += M5Cardputer.Display.fontHeight();

    // Skip spaces
    while (end < text.length() && text[end] == ' ')
      end++;

    start = end;
  }
}