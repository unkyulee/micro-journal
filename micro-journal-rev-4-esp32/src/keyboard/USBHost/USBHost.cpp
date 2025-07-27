#include "USBHost.h"
#include "app/app.h"
#include "display/display.h"
//
#include "keyboard/keyboard.h"
//
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
        _log("USB Keyboard disconnected\n");
    }

    // USB Keycode is sent when key is pressed or released
    void onKeyboard(hid_keyboard_report_t report, hid_keyboard_report_t last_report)
    {
        //
        _debug("onKeyboard %02x %02x %02x %02x %02x %02x %02x %02x\n",
               report.modifier,
               report.reserved,
               report.keycode[0],
               report.keycode[1],
               report.keycode[2],
               report.keycode[3],
               report.keycode[4],
               report.keycode[5]);

        // forward the report
        USBHost_report(report.modifier, report.reserved, report.keycode);

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
                    keyboard_HID2Ascii(report.keycode[i], report.modifier, true);
                    _log("Key Pressed: %d %d\n", report.keycode[i], report.modifier);
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
                    keyboard_HID2Ascii(last_report.keycode[i], report.modifier, false);
                    _log("Key Release: %d %d\n", last_report.keycode[i], report.modifier);
                }
            }
        }
    }
};

// Handler for USB Keyboard
MyEspUsbHost usbHost;
bool usbHostReady = false;

// this setup will be called at Rev.5 only
void USBHost_setup()
{
    // also depending on the situation should handle USB and BLE keyboards
    _log("USB Host Setup\n");

#ifndef DEBUG
    // usb keyboard setup
    usbHost.begin();
    usbHostReady = true;
#endif

#ifdef DEBUG
    _debug("USB Host is going to read keyboard input from Serial\n");
#endif
}

//
void USBHost_loop()
{

#ifndef DEBUG
    if (usbHostReady)
        // usb keyboard loop
        usbHost.task();
#endif

#ifdef DEBUG
    // receive SERIAL input and redirect to the display
    if (Serial.available())
    {
        char c = Serial.read();
        /*
                // simulate up, down, left, right
                if (c == 'w')
                    c = 20;
                if (c == 'a')
                    c = 18;
                if (c == 's')
                    c = 21;
                if (c == 'd')
                    c = 19;
        */
        if (c == 13)
            return; // ignore /r key
        _debug("Serial keyboard input %c %d\n", c, c);

        // find the scan code based on the input received

        // You can choose a key index, e.g., 0 for generic input
        display_keyboard(c, true);  // Key press
        display_keyboard(c, false); // Key release (optional, for GUI consistency)

        // emulate hid report for alphabet 
        if (c >= 'a' && c <= 'z')
        {
            uint8_t keycodes[6] = {};
            keycodes[0] = HID_KEY_A + (c - 'a');
            _debug("Serial keyboard input scancode %d\n", keycodes[0]);

            //
            USBHost_report(0, 0, keycodes);
            keycodes[0] = 0;
            USBHost_report(0, 0, keycodes);
        }
    }
#endif
}


void USBHost_report(uint8_t modifier, uint8_t reserved, uint8_t *keycodes)
{
    // forward the report to GUI
    display_keyboard_report(modifier, reserved, keycodes);
}