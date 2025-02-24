#include "hid.h"
#include "app/app.h"
#include "display/display.h"
//
#include "keyboard/keyboard.h"
//
#include "nimble/ble.h"
//
#include "button/button.h"
//
#include "usb/usb.h"
//
#include "../locale/locale.h"

// at the boot up BLE keyboard connection will be validated
bool ble_keyboard = false;

// this setup will be called at Rev.5 only
void keyboard_hid_setup()
{
    // Rev.5 will need to have Button on the front panel
    // also depending on the situation should handle USB and BLE keyboards
    app_log("HID Setup Started\n");
    if (keyboard_ble_setup())
    {
        // if BLE keyboard is connected at a boot up
        // mark it as BLE keyboard connected
        // so that it can be handled at the Loop
        ble_keyboard = true;
    }
#ifdef ENV_BUTTON
    // setup display button
    button_setup();
#endif
    // usb keyboard setup
    keyboard_usb_setup();
}

void keyboard_hid_loop()
{
    // Handle BLE keyboard when ble keyboard is connected
    if (ble_keyboard)
    {
        keyboard_ble_loop();
    }

#ifdef ENV_BUTTON
    // handle display button press
    button_loop();
#endif
    
    // usb keyboard loop
    keyboard_usb_loop();

    // When Back Space key is kept pressed
    // Send continues key presses to GUI
    if (millis() > 60 + keyboard_backspace_last() && keyboard_backspace_pressed())
    {
        keyboard_backspace_last_set(millis());

        // send backspace key
        display_keyboard('\b');
    }
}

void keyboard_hid_pressed(uint8_t keycode, uint8_t modifier)
{
    //////////////////////////////////////////
    // NUMLOCK is pressed
    if (keycode == HID_KEY_NUM_LOCK)
    {
        // mark numlock status
        keyboard_numlock_toggle();
        return;
    }

    //////////////////////////////////////////
    // CAPSLOCK is pressed
    if (keycode == HID_KEY_CAPS_LOCK)
    {
        // Mark caps lock status
        keyboard_capslock_toggle();
        return;
    }

    //////////////////////////////////////////
    // BACKSPACE
    //
    // Cancel backspace sequence when any else key is pressed
    if (keyboard_backspace_pressed())
    {
        keyboard_backspace_pressed_set(false);
    }

    // When backspace key is pressed
    // initiate backspace timer
    if (keycode == 0x2a)
    {
        keyboard_backspace_last_set(millis() + 500);
        keyboard_backspace_pressed_set(true);
    }
    //////////////////////////////////////////

    //////////////////////////////////////////
    // MENU
    //
    // ESC key is MENU button
    if (keycode == 0x29)
    {
        display_keyboard(MENU);
        return;
    }

    //////////////////////////////////////////
    // General Key Press
    //
    // Check SHIFT key pressed
    bool shift = (modifier & KEYBOARD_MODIFIER_LEFTSHIFT) || (modifier & KEYBOARD_MODIFIER_RIGHTSHIFT);

    // Check ALT key pressed
    bool alt = (modifier & KEYBOARD_MODIFIER_LEFTALT) || (modifier & KEYBOARD_MODIFIER_RIGHTALT);

    // Translate the Keycode to ASCII
    JsonDocument &app = app_status();
    String locale = app["config"]["keyboard_layout"].as<String>();
    uint8_t ascii = keyboard_keycode_ascii(locale, keycode, shift, alt);
    if (ascii != 0)
        // send key to GUI
        display_keyboard(ascii);
}

void keyboard_hid_released(uint8_t keycode, uint8_t modifier)
{
    //
    // When any key is released then cancel the backspace sequence
    //
    if (keyboard_backspace_pressed())
    {
        keyboard_backspace_pressed_set(false);
    }
}
