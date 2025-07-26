#include <Arduino.h>

// logging and status
#include "app/app.h"

// display
#include "display/display.h"

// keyboard
#include "keyboard/keyboard.h"

#ifdef BOARD_ESP32_S3
void SecondaryCore(void *pvParameters);
#endif

/*----------------------------------------------
Dual Core: First Core
First core will have slightly higher priority
to it will take the critical tasks such as
keyboard input and user interactionss
----------------------------------------------*/
void setup()
{
#ifdef DEBUG
    delay(5000);
#endif

    // initialize app
    app_setup();

    //
    display_setup();

    //
    keyboard_setup();

    if (app_ready())
    {
#ifdef BOARD_ESP32_S3
        // Start the second core task
        xTaskCreatePinnedToCore(
            SecondaryCore,   // Function to run
            "SecondaryCore", // Name
            8192,        // Stack size
            NULL,        // Parameters
            1,           // Priority
            NULL,        // Task handle
            1            // Core 1
        );
#endif
    }
}

//
void loop()
{
    // run display with main core
    if (display_core() == 0)
    {
        display_loop();
    }

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
#ifdef BOARD_PICO
void setup1()
{
    // wait until the app is ready
    while (true)
    {
        if (app_ready())
            break;
        delay(1);
    }

    _log("Secondary Core started.\n");
    delay(1000);
}

void loop1()
{
    // background tasks will be handled
    app_loop();

    // run display on the second core
    if (display_core() == 1)
        display_loop();

    // try to yield to avoid infinite loop
    yield();
}
#endif


#ifdef BOARD_ESP32_S3
// This is for ESP32 secondary core
void SecondaryCore(void *pvParameters)
{
    _log("Secondary Core started.\n");

    while (1)
    {
        // background tasks
        app_loop();

        // some conditions display render can run in second core
        if (display_core() == 1)
            display_loop();

        //
        yield();
    }
}
#endif