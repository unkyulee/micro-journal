#include "keyboard.h"
#ifdef ENV_USBHOST
#include "usb/usb.h"
#include "nimble/ble.h"
#endif

#ifdef ENV_KEYBOARD
#include "keypad/keypad.h"
#endif

//
#include "app/app.h"
#include "config/config.h"
#include "display/display.h"

//
#include "display/WordProcessor/WordProcessor.h"
#include "display/ErrorScreen/ErrorScreen.h"
#include "display/Menu/Menu.h"
#include "display/WakeUp/WakeUp.h"

bool keyboard_setup_completed = false;
bool ble_keyboard = false;

void keyboard_setup()
{
#ifdef ENV_USBHOST
    if (keyboard_ble_setup()) {
        ble_keyboard = true;
    }
    // usb keyboard setup
    keyboard_usb_setup();
#endif

#ifdef ENV_KEYBOARD
    // keypad setup
    keyboard_keypad_setup();
#endif

keyboard_setup_completed = true;
}

void keyboard_loop()
{
    if(!keyboard_setup_completed) return;

    
#ifdef ENV_USBHOST
    if (ble_keyboard) {
        keyboard_ble_loop();
    }
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
        WP_keyboard(key);
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