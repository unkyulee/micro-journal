#include "display.h"
#include "app/app.h"

// for rev.5. and rev.6.
#ifdef ILI9341_DRIVER
#include "ILI9341/display_ILI9341.h"
#endif

//
void display_setup()
{
  app_log("DISPLAY SETUP\n");

#ifdef ILI9341_DRIVER
  display_ILI9341_setup();
#endif
}

//
void display_loop()
{
#ifdef ILI9341_DRIVER
  display_ILI9341_loop();
#endif
}
