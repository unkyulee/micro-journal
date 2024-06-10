#include "keyboard.h"
#include "usb/usb.h"
#include "keypad/keypad.h"
#include "bluetooth/bluetooth.h"

//
#include "app/app.h"
#include "config/config.h"
#include "display/display.h"

//
#include "display/WordProcessor/WordProcessor.h"
#include "display/ErrorScreen/ErrorScreen.h"
#include "display/Menu/Menu.h"
#include "display/WakeUp/WakeUp.h"

void keyboard_setup()
{
#ifdef ENV_USBHOST
    // usb keyboard setup
    keyboard_usb_setup();
#endif

#ifdef ENV_KEYBOARD
    // keypad setup
    keyboard_keypad_setup();
#endif
}

void keyboard_loop()
{
#ifdef ENV_USBHOST
    // usb keyboard loop
    keyboard_usb_loop();
#endif

#ifdef ENV_KEYBOARD
    // keypad loop
    keyboard_keypad_loop();
#endif
}

// Handle Key Inputs from Various sources
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