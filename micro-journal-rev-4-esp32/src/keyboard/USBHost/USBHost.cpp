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
#ifndef DEBUG
    //
    usbHidQueue = xQueueCreate(10, sizeof(UsbHidReport));
    assert(usbHidQueue != nullptr);

    // usb keyboard setup
    usbHost.begin();
    usbHostReady = true;
    _log("USB Host Setup\n");
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

            memcpy(&prev, &report, sizeof(UsbHidReport));
        }
    }

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