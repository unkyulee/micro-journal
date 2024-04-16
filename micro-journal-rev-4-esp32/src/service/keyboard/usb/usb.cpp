#ifdef USBHOST

#include "usb.h"
#include "app/app.h"
#include "app/network/network.h"
#include "service/display/display.h"

//
#include "screens/WordProcessor/WordProcessor.h"
#include "screens/ErrorScreen/ErrorScreen.h"
#include "screens/Menu/Menu.h"

//
#include <ESP32-USB-Soft-Host.h>

#define DP_P0 12 // always enabled
#define DM_P0 14 // always enabled

#define MENU_KEY 25
#define BACK_KEY 26

usb_pins_config_t USB_Pins_Config = {DP_P0, DM_P0};

// initialize USB HOST
void keyboard_setup()
{
    USH.setOnConfigDescCB(Default_USB_ConfigDescCB);
    USH.setOnIfaceDescCb(Default_USB_IfaceDescCb);
    USH.setOnHIDDevDescCb(Default_USB_HIDDevDescCb);
    USH.setOnEPDescCb(Default_USB_EPDescCb);

    USH.init(USB_Pins_Config, keyboard_connected, keyboard_print);

    // setup two buttons on the device
    pinMode(MENU_KEY, INPUT_PULLUP);
    pinMode(BACK_KEY, INPUT_PULLUP);
}

///
void keyboard_loop()
{
    static unsigned int last = 0;
    if (millis() - last > 1000)
    {
        //
        last = millis();

        // check menu key and back key press
        int menuKey = digitalRead(MENU_KEY);
        int backKey = digitalRead(BACK_KEY);

        // 0 means pressed
        // 1 means released
        if (menuKey == 0)
        {
            // open up the menu
            JsonDocument &app = app_status();
            app["screen"] = MENUSCREEN;
        }

        if (backKey == 0)
        {
            // stop flag
            JsonDocument &app = app_status();
            app["stop"] = true;

            // trigger stop action
            Menu_keyboard(0);
        }
    }
}

// USB keyboard scan code to ASCII mapping for a standard US keyboard
char map_usb_hid_to_ascii(uint8_t key_code, bool shift_pressed)
{
    // Check if the key code is within the range of printable ASCII characters
    if (key_code >= 0x04 && key_code <= 0x1d)
    {
        // If Shift is pressed, map to uppercase letters, otherwise lowercase
        if (shift_pressed)
            return key_code + 'A' - 0x04;
        else
            return key_code + 'a' - 0x04;
    }
    else if (key_code >= 0x1e && key_code <= 0x27)
    {
        // If Shift is pressed, map to symbols above numbers
        if (shift_pressed)
        {
            switch (key_code)
            {
            case 0x1e:
                return '!'; // 1
            case 0x1f:
                return '@'; // 2
            case 0x20:
                return '#'; // 3
            case 0x21:
                return '$'; // 4
            case 0x22:
                return '%'; // 5
            case 0x23:
                return '^'; // 6
            case 0x24:
                return '&'; // 7
            case 0x25:
                return '*'; // 8
            case 0x26:
                return '('; // 9
            case 0x27:
                return ')'; // 0
            default:
                return 0; // Invalid key code
            }
        }
        else
        {
            // If Shift is not pressed, map to numbers
            return key_code + '1' - 0x1e;
        }
    }
    else if (key_code == 0x2C)
    {
        // Handle space key
        return ' ';
    }
    else if (key_code == 0x36)
    {
        // Handle comma key
        if(shift_pressed) return '<';
        return ',';
    }
    else if (key_code == 0x37)
    {
        // Handle period key
        if(shift_pressed) return '>';
        return '.';
    }
    else if (key_code == 0x38)
    {
        // Handle slash key
        if(shift_pressed) return '?';
        return '/';
    }
    else if (key_code == 0x2D)
    {
        if(shift_pressed) return '_';
        return '-';
    }
    else if (key_code == 0x2E)
    {
        if(shift_pressed) return '+';
        return '=';
    }
    else if (key_code == 0x2F)
    {
        if(shift_pressed) return '{';
        return '[';
    }
    else if (key_code == 0x30)
    {
        if(shift_pressed) return '}';
        return ']';
    }
    else if (key_code == 0x31)
    {
        if(shift_pressed) return '|';
        return '\\';
    }
    else if (key_code == 0x33)
    {
        if(shift_pressed) return ':';
        return ';';
    }
    else if (key_code == 0x34)
    {
        // Handle left bracket key
        if(shift_pressed) return '\"';
        return '\'';
    }
    else if (key_code == 0x35)
    {
        // Handle right bracket key
        if(shift_pressed) return '~';
        return '`';
    }
    else if (key_code == 0x2A)
    {
        return BACKSPACE;
    }
    else if (key_code == 0x28)
    {
        // Handle enter key
        return '\n';
    }
    else
    {
        // Key code doesn't correspond to a printable ASCII character
        Serial.printf("Not registered: %02x\n", key_code);
        return 0; // Return null character to indicate invalid key
    }
}

static void keyboard_print(uint8_t usbNum, uint8_t byte_depth, uint8_t *data, uint8_t data_len)
{
    /*
    printf("in: ");
    for (int k = 0; k < data_len; k++)
    {
        printf("0x%02x ", data[k]);
    }
    printf("\n");
    */

    
    // previous data buffer
    static uint8_t previous[8] = {0};

    // 0x02 and 0x22 are shift keys
    bool shift = (data[0] & 0x02) || (data[0] & 0x22);
    for (int i = 2; i < data_len; i++)
    {
        // report only when it is pressed
        if (data[i] != 0 && previous[i] == 0 && data[i] < 128)
        {
            // Type
            char key = map_usb_hid_to_ascii(data[i], shift);
            if (key == 0)
                continue;

            //
            // depending on the screen
            // send the keystrokes
            JsonDocument &app = app_status();
            int screen = app["screen"].as<int>();

            if (screen == WORDPROCESSOR)
            {
                // send the key stroke to word processor
                WP_keyboard((char)key);
            }
            else if (screen == MENUSCREEN)
            {
                Menu_keyboard((char)key);
            }
            else if (screen == ERRORSCREEN)
            {
                ErrorScreen_keyboard((char)key);
            }
        }
    }

    // keep the previous
    memcpy(previous, data, sizeof(previous));    
}

static void keyboard_connected(uint8_t usbNum, void *dev)
{
    sDevDesc *device = (sDevDesc *)dev;
    printf("New device detected on USB#%d\n", usbNum);
    printf("desc.bcdUSB             = 0x%04x\n", device->bcdUSB);
    printf("desc.bDeviceClass       = 0x%02x\n", device->bDeviceClass);
    printf("desc.bDeviceSubClass    = 0x%02x\n", device->bDeviceSubClass);
    printf("desc.bDeviceProtocol    = 0x%02x\n", device->bDeviceProtocol);
    printf("desc.bMaxPacketSize0    = 0x%02x\n", device->bMaxPacketSize0);
    printf("desc.idVendor           = 0x%04x\n", device->idVendor);
    printf("desc.idProduct          = 0x%04x\n", device->idProduct);
    printf("desc.bcdDevice          = 0x%04x\n", device->bcdDevice);
    printf("desc.iManufacturer      = 0x%02x\n", device->iManufacturer);
    printf("desc.iProduct           = 0x%02x\n", device->iProduct);
    printf("desc.iSerialNumber      = 0x%02x\n", device->iSerialNumber);
    printf("desc.bNumConfigurations = 0x%02x\n", device->bNumConfigurations);
}

#endif