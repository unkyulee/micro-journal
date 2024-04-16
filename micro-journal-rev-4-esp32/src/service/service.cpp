#include "service.h"
#include "display/display.h"
#include "keyboard/keyboard.h"


void service_setup()
{
    //
    display_setup();

    //
    keyboard_setup();
}

// place the service loops that is critical and should be performed in timely manner
void service_loop()
{
    //
    display_loop();

    //
    keyboard_loop();
}
