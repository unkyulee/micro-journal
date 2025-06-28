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
    delay(3000);

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
}

void loop1()
{
    // background tasks will be handled
    app_loop();

    // try to yield to avoid infinite loop
    yield();
}
