#include <Arduino.h>

// logging and status
#include "app/app.h"

// display
#include "display/display.h"

// keyboard
#include "keyboard/keyboard.h"

/*----------------------------------------------
Dual Core: First Core
First core will have slightly higher priority
to it will take the critical tasks such as
keyboard input and user interactionss
----------------------------------------------*/
void setup()
{
    // initialize app
    app_setup();

    //
    display_setup();

    //
    keyboard_setup();
}

//
void loop()
{
    //
    if (display_core() == 0)
        display_loop();

    //
    keyboard_loop();

    // try to yield to avoid infinite loop
    yield();
}

/*----------------------------------------------
Dual Core: Second Core
Second core will contain tasks that can get blocked
and slowed and yet has still less impact.
Such as background tasks.
----------------------------------------------*/

void setup1()
{
    // wait until the app is ready
    while(true) {
        if(app_ready()) break;
        delay(1);
    }

    _log("Core 1 started.\n");
    delay(1000);
}

void loop1()
{    
    // background tasks will be handled
    app_loop();

    //
    if (display_core() == 1)
        display_loop();

    // try to yield to avoid infinite loop
    yield();
}
