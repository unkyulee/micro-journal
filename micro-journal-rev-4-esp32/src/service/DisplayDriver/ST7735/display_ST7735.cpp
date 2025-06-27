#include "display_ST7735.h"
#include "display/display.h"
#include "app/app.h"

// screens
#include "service/DisplayDriver/ST7735/GUI/ErrorScreen/ErrorScreen.h"
#include "service/DisplayDriver/ST7735/GUI/WordProcessor/WordProcessor.h"

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
  app_log("Display ST7735 setup called\n");

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
    
    // TBD: Wakeup Screen
    // Word Processor
    else if (screen == WAKEUPSCREEN || screen == WORDPROCESSOR)
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

    /*
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
    */

    
  }
}

// Redirect the key press to the current GUI
void display_ST7735_keyboard(char key)
{
}