#include "keyboard.h"
#include "usb/usb.h"
#include "keypad/keypad.h"
#include "bluetooth/bluetooth.h"

//
#include "app/app.h"
#include "app/config/config.h"
#include "service/display/display.h"

//
#include "screens/WordProcessor/WordProcessor.h"
#include "screens/ErrorScreen/ErrorScreen.h"
#include "screens/Menu/Menu.h"
#include "screens/WakeUp/WakeUp.h"

void keyboard_setup()
{
    // usb keyboard setup
    keyboard_usb_setup();

    // bluetooth keyboard setup
    keyboard_bluetooth_setup();
}

void keyboard_loop()
{
    // usb keyboard loop
    keyboard_usb_loop();

    // bluetooth keyboard setup
    keyboard_bluetooth_loop();
}
void keyboard_key(char key)
{
    //
    // depending on the screen
    // send the keystrokes
    JsonDocument &app = app_status();
    int screen = app["screen"].as<int>();

    if (screen == WORDPROCESSOR)
    {
        // send the key stroke to word processor
        WordProcessor::getInstance(nullptr, nullptr).keyboard(key);
    }
    else if (screen == MENUSCREEN)
    {
        Menu_keyboard(key);
    }
    else if (screen == ERRORSCREEN)
    {
        ErrorScreen_keyboard(key);
    }
    else if (screen == WAKEUPSCREEN || screen == SLEEPSCREEN)
    {
        WakeUp_keyboard(key);
    }
}