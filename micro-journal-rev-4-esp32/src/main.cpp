#include <Arduino.h>

//
#include "app/app.h"
#include "SD/sd.h"
#include "config/config.h"
#include "display/display.h"
#include "keyboard/keyboard.h"

// Dual Core Support
TaskHandle_t Task0;
void Core0(void *parameter);

void setup()
{
    // Setup serial for ESP32
#ifdef ILI9341_DRIVER
    app_log("Device Started. Baud rate: %d\n", 9600);
    Serial.begin(9600);
#endif
#ifdef ENV_EPAPER
    app_log("Device Started. Baud rate: %d\n", 115200);
    Serial.begin(115200);
#endif

    // SD must be initialized before display
    SD_setup();

    // initialize config
    config_setup();

    // keyboard setup
    keyboard_setup();

    //
    display_setup();

    //
    xTaskCreatePinnedToCore(
        Core0,   // Function to implement the task
        "Core0", // Name of the task
        10000,   // Stack size in words
        NULL,    // Task input parameter
        0,       // Priority of the task
        &Task0,  // Task handle.
        0);      // Core where the task should run

}

// Main loop is ignored as the tasks are separated per core
void loop()
{
    //
    display_loop();

    //
    yield();
}

void Core0(void *parameter)
{
    app_log("Core0 Task Start\n");
    while (true)
    {
        //
        keyboard_loop();

        //
        yield();
    }
}
