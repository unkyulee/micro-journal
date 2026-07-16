#include "USBHost.h"
#include "app/app.h"
#include "display/display.h"
//
#include "keyboard/keyboard.h"
//
#include <EspUsbHost.h>

struct UsbHidReport
{
    uint8_t modifier;
    uint8_t reserved;
    uint8_t keycode[6];
};

static QueueHandle_t usbHidQueue;

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
        /*
        _log("onKeyboard %02x %02x %02x %02x %02x %02x %02x %02x\n",
               report.modifier,
               report.reserved,
               report.keycode[0],
               report.keycode[1],
               report.keycode[2],
               report.keycode[3],
               report.keycode[4],
               report.keycode[5]);
        */

        // forward the report
        USBHost_report(report.modifier, report.reserved, report.keycode);

        // Copy the report into a local struct
        UsbHidReport r;
        r.modifier = report.modifier;
        r.reserved = report.reserved;
        memcpy(r.keycode, report.keycode, 6);

        // Send to queue from ISR-safe context
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xQueueSendFromISR(usbHidQueue, &r, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
};

// Handler for USB Keyboard
MyEspUsbHost usbHost;
bool usbHostReady = false;

// this setup will be called at Rev.5 only
void USBHost_setup()
{
#ifndef USE_SERIAL_KEYBOARD
    //
    usbHidQueue = xQueueCreate(10, sizeof(UsbHidReport));
    assert(usbHidQueue != nullptr);

    // usb keyboard setup
    usbHost.begin();
    usbHostReady = true;
    _log("USB Host Setup\n");
#endif

#ifdef USE_SERIAL_KEYBOARD
    _debug("USB Host is going to read keyboard input from Serial\n");
#endif
}

//
void USBHost_loop()
{

#ifndef USE_SERIAL_KEYBOARD
    if (usbHostReady)
        usbHost.task();

    if (usbHidQueue != nullptr)
    {
        UsbHidReport report;
        static UsbHidReport prev = {0};

        // Non-blocking receive
        while (xQueueReceive(usbHidQueue, &report, 0) == pdTRUE)
        {
            // Key Pressed
            for (int i = 0; i < 6; i++)
            {
                if (report.keycode[i] != 0)
                {
                    bool newKey = true;
                    for (int j = 0; j < 6; j++)
                        if (prev.keycode[j] == report.keycode[i])
                            newKey = false;
                    if (newKey)
                        keyboard_HID2Ascii(report.keycode[i], report.modifier, true);
                }
            }

            // Key Released
            for (int i = 0; i < 6; i++)
            {
                if (prev.keycode[i] != 0)
                {
                    bool released = true;
                    for (int j = 0; j < 6; j++)
                        if (prev.keycode[i] == report.keycode[j])
                            released = false;
                    if (released)
                        keyboard_HID2Ascii(prev.keycode[i], report.modifier, false);
                }
            }

            // Physical Alt toggles Hangul/English in the Korean locale
            // (see keyboard_keycode_ascii_ko). Held alone, Alt never shows
            // up in the 6-key rollover array above - it only ever appears
            // in the modifier byte - so it needs its own edge-detect here.
            const uint8_t ALT_BITS = 0x04 | 0x40; // left + right alt
            bool altPrev = prev.modifier & ALT_BITS;
            bool altNow = report.modifier & ALT_BITS;
            if (altNow != altPrev)
                keyboard_HID2Ascii(0, report.modifier, altNow);

            memcpy(&prev, &report, sizeof(UsbHidReport));
        }
    }

#endif

#ifdef USE_SERIAL_KEYBOARD
    // receive SERIAL input and redirect to the display
    if (Serial.available())
    {
        char c = Serial.read();

        //if (c == 'r') c = 5; // emulate refresh key
        if (c == '`') c = 5; // emulate refresh key
        if (c == 13) return; // ignore /r key

         //
        _log("Serial keyboard input %c %d\n", c, c);

        // Letters must travel the SAME path a real USB key does -
        // HID keycode -> keyboard_HID2Ascii -> locale -> ascii - otherwise
        // the layout (e.g. Korean) is bypassed and the raw latin letter is
        // typed instead of the mapped jamo. Uppercase is expressed as the
        // letter's keycode plus the left-shift modifier, exactly like real
        // hardware, so 'R' -> ㄲ still works.
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
        {
            bool upper = (c >= 'A' && c <= 'Z');
            uint8_t keycode = HID_KEY_A + (upper ? c - 'A' : c - 'a');
            uint8_t modifier = upper ? 0x02 : 0x00; // 0x02 = left shift

            _log("Serial keyboard input scancode %d shift %d\n", keycode, upper);

            keyboard_HID2Ascii(keycode, modifier, true);
            keyboard_HID2Ascii(keycode, modifier, false);
        }
        else
        {
            // non-letter keys (arrows, backspace, enter, control codes)
            // are not remapped by the locale - send them straight through
            display_keyboard(c, true);  // Key press
            display_keyboard(c, false); // Key release
        }
    }
#endif
}

void USBHost_report(uint8_t modifier, uint8_t reserved, uint8_t *keycodes)
{
    // forward the report to GUI
    display_keyboard_report(modifier, reserved, keycodes);
}