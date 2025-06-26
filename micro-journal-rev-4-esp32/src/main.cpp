#include <Arduino.h>

//
#include "app/app.h"
#include "SD/sd.h"
#include "config/config.h"
#include "display/display.h"
#include "keyboard/keyboard.h"

#ifdef BOARD_ESP32_S3
#include "service/Wifi/WifiService.h"
#endif

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

    //
    display_setup();
}

// Main loop is ignored as the tasks are separated per core
void loop()
{
    //
    display_loop();

    //
    yield();
}

void setup1()
{
    // keyboard setup
    keyboard_setup();
}

void loop1()
{
    while (true)
    {
        //
        keyboard_loop();
#ifdef BOARD_ESP32_S3
        // check for background task request
        wifi_service_loop();
#endif
        //
        yield();
    }
}
