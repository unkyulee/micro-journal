#include "display.h"
#include "app/app.h"
#include "service/SD/sd.h"
#include "service/WordProcessor/WordProcessor.h"

// Invoke library, pins defined in platformio.ini
TFT_eSPI tft = TFT_eSPI();

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
  app["screen"] = WORDPROCESSOR;
}

void display_loop()
{
  static unsigned int last = millis();
  if (millis() - last > 100)
  {
    last = millis();

    JsonDocument &app = app_status();
    int screen = app["screen"].as<int>();

    if (screen == WORDPROCESSOR)
    {
      WP_render(&tft);
    }
  }
}
