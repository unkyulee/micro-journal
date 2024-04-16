#include "display.h"
#include "app/app.h"
#include "screens/WordProcessor/WordProcessor.h"
#include "screens/ErrorScreen/ErrorScreen.h"
#include "screens/Menu/Menu.h"

// Invoke library, pins defined in platformio.ini
TFT_eSPI tft = TFT_eSPI();

//
void display_setup()
{
  // Initialise the TFT screen
  tft.begin();

  //
  tft.setRotation(3);

  // Fill screen with grey so we can see the effect of printing with and without
  // a background colour defined
  tft.fillScreen(TFT_BLACK);

  // set the initial screen to word processor
  JsonDocument &app = app_status();

  // check SD card status
  if (app["SD"].as<bool>() != true)
  {
    app["screen"] = ERRORSCREEN;
  }
  else
  {
    app["screen"] = WORDPROCESSOR;
  }
}

//
int screen_prev = -1;
void display_loop()
{
  static unsigned int last = millis();
  if (millis() - last > 30)
  {
    last = millis();

    JsonDocument &app = app_status();
    int screen = app["screen"].as<int>();

    // WORD PROCESSOR
    if (screen == WORDPROCESSOR)
    {
      // setup only once
      if (screen != screen_prev)
        WordProcessor::getInstance(&tft).setup();

      // loop
      WordProcessor::getInstance(&tft).render();
    }

    // ERROR SCREEN
    else if (screen == ERRORSCREEN)
    {
      // setup only once
      if (screen != screen_prev)
        ErrorScreen_setup(&tft);

      // loop
      ErrorScreen_render(&tft);
    }

    // MENU SCREEN
    else if (screen == MENUSCREEN)
    {
      // setup only once
      if (screen != screen_prev)
        Menu_setup(&tft);

      // loop
      Menu_render(&tft);
    }

    //
    screen_prev = screen;
  }
}
