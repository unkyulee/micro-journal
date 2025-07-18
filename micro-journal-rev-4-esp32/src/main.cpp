#include <Arduino.h>

// logging and status
#include "app/app.h"

// display
#include "display/display.h"

// keyboard
#include "keyboard/keyboard.h"

#ifdef BOARD_ESP32_S3
void TaskCore1(void *pvParameters);
#endif

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

    if (app_ready())
    {
        //
        keyboard_setup();

#ifdef BOARD_ESP32_S3
        // Start the second core task
        xTaskCreatePinnedToCore(
            TaskCore1,   // Function to run
            "TaskCore1", // Name
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
    //
#ifdef BOARD_PICO
    // in PICO core is 0
    if (display_core() == 0)
#endif

#ifdef BOARD_ESP32_S3
        // in ESP32 main core ID is 1
        if (display_core() == 1)
#endif
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
#endif

//
void TaskCore1(void *pvParameters)
{
    while (1)
    {
        app_loop();
        if (display_core() == 0)
            display_loop();
        yield();
    }
}
