#include <Arduino.h>

// logging and status
#include "app/app.h"

// display
//#include "display/display.h"

// keyboard
//#include "keyboard/keyboard.h"

/*----------------------------------------------
Dual Core: First Core
First core will have slightly higher priority
to it will take the critical tasks such as
keyboard input and user interactionss
----------------------------------------------*/
void setup()
{
    // keyboard setup
    //keyboard_setup();
}

// Main loop is ignored as the tasks are separated per core
void loop()
{
    //
    //keyboard_loop();
}

/*----------------------------------------------
Dual Core: Second Core
Second core will contain tasks that can get blocked 
and slowed and yet has still less impact. 
Such as display updates and background tasks.
----------------------------------------------*/
void setup1()
{
    // initialize app
    app_setup();

    //
    //display_setup();
}

void loop1()
{
    //
    app_loop();

    //
    //display_loop();
}
