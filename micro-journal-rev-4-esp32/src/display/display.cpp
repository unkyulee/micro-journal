#include "display.h"
#include "app/app.h"

// for rev.5. and rev.6.
#ifdef ENV_ILI9341
#include "ILI9341/display_ILI9341.h"
#endif
#ifdef ENV_EPAPER
#include "EPD/display_EPD.h"
#endif

//
void display_setup()
{
#ifdef ENV_ILI9341
  app_log("ILI9341 Display SETUP\n");
  display_ILI9341_setup();
#endif
#ifdef ENV_EPAPER
  app_log("E-ink Setup\n");
  display_EPD_setup();
#endif

  // set the initial screen to word processor
  JsonDocument &app = app_status();
  int screen = app["screen"].as<int>();
  app_log("Display initializing with screen %d\n", screen);
  if (screen == 0)
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
      app["screen_prev"] = -1;
    }
    else
    {
      //
      app["screen"] = WAKEUPSCREEN;
      app["screen_prev"] = -1;
    }
  }
  app_log("Display loading screen %d\n", app["screen"].as<int>());
}

//
void display_loop()
{
#ifdef ENV_ILI9341
  display_ILI9341_loop();
#endif
#ifdef ENV_EPAPER
  display_EPD_loop();
#endif
}

void display_keyboard(char key)
{
#ifdef ENV_ILI9341
  display_ILI9341_keyboard(key);
#endif
#ifdef ENV_EPAPER
  display_EPD_keyboard(key);
#endif
}
