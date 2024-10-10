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
  app_log("DISPLAY SETUP\n");

#ifdef ENV_ILI9341
  display_ILI9341_setup();
#endif
#ifdef ENV_EPAPER
  display_EPD_setup();
#endif
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