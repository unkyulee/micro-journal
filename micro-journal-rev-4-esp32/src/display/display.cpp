#include "display.h"
#include "app/app.h"

// ILI9341 DISPLAY
#ifdef ILI9341_DRIVER
#include "service/DisplayDriver/ILI9341/display_ILI9341.h"
#endif

// EPD DISPLAY
#ifdef LILYGO_T5_EPD47_S3
#include "service/DisplayDriver/EPD/display_EPD.h"
#endif

// Lilygo 0.96 LCD Display
#ifdef ST7735_DRIVER
#include "display/ST7735/display_ST7735.h"
#endif

//
void display_setup()
{
#ifdef ILI9341_DRIVER
  display_ILI9341_setup();
#endif

#ifdef LILYGO_T5_EPD47_S3
  display_EPD_setup();
#endif

#ifdef ST7735_DRIVER
  display_ST7735_setup();
#endif

  // Identifying which screen to show
  JsonDocument &app = status();
  int screen = app["screen"].as<int>();
  app["screen_prev"] = -1;

  //  
  _log("Display initializing with screen %d\n", screen);

  //
  if (screen != ERRORSCREEN)
  {
    //
    // if screen is not specified
    // then load the wake animation then the word processor
    //
    bool disabled = app["config"]["wakeup_animation_disabled"].as<bool>();
    if (disabled)
    {
      // show the word processor immediately when wakeup is disabled
      app["screen"] = WORDPROCESSOR;
    }
    else
    {
      // show wakeup screen
      app["screen"] = WAKEUPSCREEN;
    }
  }

  //
  _log("Display loading screen %d\n", app["screen"].as<int>());
}

//
void display_loop()
{
#ifdef ILI9341_DRIVER
  display_ILI9341_loop();
#endif

#ifdef LILYGO_T5_EPD47_S3
  display_EPD_loop();
#endif

#ifdef ST7735_DRIVER
  display_ST7735_loop();
#endif
}

void display_keyboard(char key)
{
#ifdef ILI9341_DRIVER
  display_ILI9341_keyboard(key);
#endif

#ifdef LILYGO_T5_EPD47_S3
  display_EPD_keyboard(key);
#endif

#ifdef ST7735_DRIVER
  display_ST7735_keyboard(key);
#endif
}
