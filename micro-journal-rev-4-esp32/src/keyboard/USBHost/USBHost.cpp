#include "USBHost.h"
#include "app/app.h"
#include "display/display.h"
//
#include "keyboard/keyboard.h"
//
#include "keyboard/Locale/locale.h"

#include <EspUsbHost.h>

class MyEspUsbHost : public EspUsbHost
{
    void onReceive(const usb_transfer_t *transfer)
    {
        JsonDocument &app = status();
        app["hid_usb"] = true;
    }
    void onGone(const usb_host_client_event_msg_t *eventMsg)
    {
        JsonDocument &app = status();
        app["hid_usb"] = false;
    }

    // USB Keycode is sent when key is pressed or released
    void onKeyboard(hid_keyboard_report_t report, hid_keyboard_report_t last_report)
    {
        //
        _debug("onKeyboard::%02x %02x %02x %02x %02x %02x %02x %02x\n",
               report.modifier,
               report.reserved,
               report.keycode[0],
               report.keycode[1],
               report.keycode[2],
               report.keycode[3],
               report.keycode[4],
               report.keycode[5]);

        // Key Pressed
        for (int i = 0; i < 6; i++)
        {
            if (report.keycode[i] != 0)
            {
                // check if the same key appear in the previous report
                bool newkey = true;
                for (int j = 0; j < 6; j++)
                {
                    if (last_report.keycode[j] == report.keycode[i])
                    {
                        newkey = false;
                        break;
                    }
                }

                // otherwise register new key press
                if (newkey)
                {
                    // handle key pressed
                    USBHost_keyboard(report.keycode[i], report.modifier, true);
                }
            }
        }

        // Key Released
        for (int i = 0; i < 6; i++)
        {
            if (last_report.keycode[i] != 0)
            {
                bool key_released = true;
                for (int j = 0; j < 6; j++)
                {
                    if (last_report.keycode[i] == report.keycode[j])
                    {
                        key_released = false;
                        break;
                    }
                }
                if (key_released)
                {
                    USBHost_keyboard(report.keycode[i], report.modifier, false);
                }
            }
        }
    }
};

// Handler for USB Keyboard
MyEspUsbHost usbHost;

// this setup will be called at Rev.5 only
void USBHost_setup()
{
    // also depending on the situation should handle USB and BLE keyboards
    _log("USB Host Setup\n");

#ifndef DEBUG
    // usb keyboard setup
    usbHost.begin();
#endif

#ifdef DEBUG
    _debug("USB Host is going to read keyboard input from Serial\n");
#endif
}

//
void USBHost_loop()
{

#ifndef DEBUG
    // usb keyboard loop
    usbHost.task();
#endif

#ifdef DEBUG
    // receive SERIAL input and redirect to the display
    if (Serial.available())
    {
        char c = Serial.read();
        _debug("Serial keyboard input %c %d\n", c, c);
        // You can choose a key index, e.g., 0 for generic input
        display_keyboard(c, true);  // Key press
        display_keyboard(c, false); // Key release (optional, for GUI consistency)
    }
#endif
}

//
void USBHost_keyboard(uint8_t keycode, uint8_t modifier, bool pressed)
{
     //////////////////////////////////////////
    // CAPSLOCK is pressed
    if (keycode == HID_KEY_CAPS_LOCK && pressed)
    {
        // Mark caps lock status
        keyboard_capslock_toggle();
        return;
    }

    //////////////////////////////////////////
    // MENU
    // ESC key is MENU button
    if (keycode == 0x29)
    {
        display_keyboard(MENU, pressed, 69);
        return;
    }

    //////////////////////////////////////////
    // General Key Press
    // Check SHIFT key pressed
    bool shift = (modifier & KEYBOARD_MODIFIER_LEFTSHIFT) || (modifier & KEYBOARD_MODIFIER_RIGHTSHIFT);

    // Check ALT key pressed
    bool alt = (modifier & KEYBOARD_MODIFIER_LEFTALT) || (modifier & KEYBOARD_MODIFIER_RIGHTALT);

    // Translate the Keycode to ASCII
    JsonDocument &app = status();
    String locale = app["config"]["keyboard_layout"].as<String>();
    uint8_t ascii = keyboard_keycode_ascii(locale, keycode, shift, alt);
    if (ascii != 0)
        // send key to GUI
        display_keyboard(ascii, pressed);
}
