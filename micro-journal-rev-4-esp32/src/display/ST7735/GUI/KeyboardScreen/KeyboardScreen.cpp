#include "KeyboardScreen.h"
#include "app/app.h"
#include "display/display.h"

#include <Keyboard.h>

//
void KeyboardScreen_setup(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    ptft->fillScreen(TFT_BLACK);
}

//
void KeyboardScreen_render(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    ///
    ptft->setTextSize(2); // Adjust text size as needed
    ptft->setTextColor(TFT_WHITE, TFT_BLACK);

    // Draw text in the middle of the screen
    ptft->setCursor(0, 0);
    ptft->print("\n");
    ptft->print("\n");
    ptft->print("USB Keyboard ");

    // Setup USB Keyboard

    // TBD: Play GIF
}

/*
Find the keycode from
https://github.com/arduino-libraries/Keyboard/blob/master/src/Keyboard.h

LAYER KEY: #define KEY_F24           0xFB
*/

// layers
// prettier-ignore
int _usb_keyboard_layers[2][72] = {

    {// normal layers
     KEY_ESC, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b', KEY_DELETE,
     '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', KEY_PAGE_UP,
     KEY_CAPS_LOCK, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '\\', KEY_PAGE_DOWN,
     KEY_LEFT_SHIFT, '`', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', KEY_LEFT_SHIFT, KEY_UP_ARROW, KEY_END,
     KEY_LEFT_CTRL, KEY_LEFT_GUI, KEY_LEFT_ALT, ' ', KEY_RIGHT_ALT, KEY_F24, KEY_HOME, KEY_LEFT_ARROW, KEY_DOWN_ARROW, KEY_RIGHT_ARROW,
     0},

    {// extra layer
     KEY_ESC, KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6, KEY_F7, KEY_F8, KEY_F9, KEY_F10, KEY_F11, KEY_F12, '\b', KEY_DELETE,
     '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', KEY_PRINT_SCREEN,
     KEY_CAPS_LOCK, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '\\', KEY_PAGE_DOWN,
     KEY_LEFT_SHIFT, '`', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', KEY_LEFT_SHIFT, KEY_UP_ARROW, KEY_END,
     KEY_LEFT_CTRL, KEY_LEFT_GUI, KEY_LEFT_ALT, ' ', KEY_RIGHT_ALT, KEY_F24, KEY_HOME, KEY_LEFT_ARROW, KEY_DOWN_ARROW, KEY_RIGHT_ARROW,
     0},

};

void KeyboardScreen_keyboard(int key, bool pressed, int index)
{
    JsonDocument &app = status();
    _debug("KeyboardScreen_keyboard - %d %d %d\n", key, pressed, index);

    //
    if (index == 69)
    {
        if (!pressed)
        {
            _debug("KeyboardScreen_keyboard - Received MENU Key\n");
            app["screen"] = MENUSCREEN;
        }

        return;
    }

    // Translate the key
    static int layer = 0;
    if (_usb_keyboard_layers[0][index] == KEY_F24)
    {
        if (pressed)
            layer = 1;
        else
            layer = 0;

        return;
    }

    //
    int usbKey = _usb_keyboard_layers[layer][index];

    // Send out as a USB Keyboard Key Press
    if (pressed)
    {
        Keyboard.press(usbKey); // Press the key
    }
    else
    {
        Keyboard.release(usbKey); // Release the key
    }
}