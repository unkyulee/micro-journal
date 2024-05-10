#include "usb.h"
#include "app/app.h"
#include "app/config/config.h"
#include "service/display/display.h"
#include "service/keyboard/ascii/ascii.h"

//
#include "screens/WordProcessor/WordProcessor.h"
#include "screens/ErrorScreen/ErrorScreen.h"
#include "screens/Menu/Menu.h"
#include "screens/WakeUp/WakeUp.h"

void keyboard_bluetooth_setup()
{
    //
    JsonDocument &app = app_status();
    bool enabled = app["config"]["bluetooth_enabled"].as<bool>();

    if (enabled)
    {
        app_log("Init Bluetooth Keyboard\n");
    }
}

void keyboard_bluetooth_loop() {}
