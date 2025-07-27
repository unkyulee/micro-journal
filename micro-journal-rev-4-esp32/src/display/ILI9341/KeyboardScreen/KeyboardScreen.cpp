#include "KeyboardScreen.h"
#include "app/app.h"
#include "display/display.h"
#include "service/Editor/Editor.h"
#include "keyboard/keyboard.h"

//
#include <BleKeyboard.h>

BleKeyboard bleKeyboard;
int keyboardConnectedPrev = -1;

/*
Find the keycode from
https://github.com/arduino-libraries/Keyboard/blob/master/src/Keyboard.h

LAYER KEY: #define KEY_F24           0xFB
*/
int _usb_keyboard_layers[3][48] = {

    {// normal layers
     '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '\b',
     KEY_ESC, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'',
     KEY_LEFT_SHIFT, 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', '\n',
     0, KEY_LEFT_CTRL, KEY_LEFT_ALT, KEY_LEFT_GUI, KEY_F24, ' ', ' ', KEY_F23, KEY_LEFT_ARROW, KEY_DOWN_ARROW, KEY_UP_ARROW, KEY_RIGHT_ARROW},

    {// lower
     '~', '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', KEY_DELETE,
     KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6, KEY_F7, KEY_F8, KEY_F9, KEY_F10, KEY_F11, KEY_F12,
     KEY_LEFT_SHIFT, '_', '+', '{', '}', '|', 'n', 'm', ',', '.', '/', '\n',
     MENU, KEY_LEFT_CTRL, KEY_LEFT_ALT, KEY_LEFT_GUI, KEY_F24, ' ', ' ', KEY_F23, KEY_HOME, KEY_PAGE_DOWN, KEY_PAGE_UP, KEY_END},

    {// raise
     '`', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '\b',
     KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6, KEY_F7, KEY_F8, KEY_F9, KEY_F10, KEY_F11, KEY_F12,
     KEY_LEFT_SHIFT, '-', '=', '[', ']', '\\', 'n', 'm', ',', '.', '/', '\n',
     MENU, KEY_LEFT_CTRL, KEY_LEFT_ALT, KEY_LEFT_GUI, KEY_F24, ' ', ' ', KEY_F23, KEY_PRTSC, KEY_PAGE_DOWN, KEY_PAGE_UP, KEY_END},
};

//
void KeyboardScreen_setup(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    // clear screen
    ptft->fillScreen(TFT_BLACK);

    // reset flags
    keyboardConnectedPrev = -1;

    // load keyboard layout
    // Load Custom Keybaord Layout
    const char *keys[] = {"main", "lower", "raise"};
    keyboard_config_load(
        "/keyboard_usb.json",
        (int *)_usb_keyboard_layers,
        48,
        keys,
        2);

    // Setup Bluetooth Keyboard
    bleKeyboard.setName("Micro Journal 6");
    bleKeyboard.begin();
    _log("Bluetooth Keyboard Started\n");
}

//
void KeyboardScreen_render(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    bool keyboardConnected = bleKeyboard.isConnected();

    // clear screen if status changed
    if (keyboardConnected != keyboardConnectedPrev)
    {
        ptft->fillScreen(TFT_BLACK);
        keyboardConnectedPrev = keyboardConnected;
    }

    ///
    ptft->setCursor(0, 0, 2);
    ptft->setTextSize(1); // Adjust text size as needed
    ptft->setTextColor(TFT_WHITE, TFT_BLACK);

    // Draw text in the middle of the screen
    ptft->setCursor(0, 0, 2);
    ptft->println("Bluetooth Keyboard Mode");
    ptft->println();

    //
    if (keyboardConnected)
    {
        ptft->println("Keyboard is connected");
        ptft->println();
    }
    else
    {
        ptft->println("Waiting for connection ...");
        ptft->println();
    }

    ptft->println("Turn off the device to end the writing session");
    ptft->println("Pressing V + Left Shift (MENU) will send the text");
}

// keyboard message will come from Rev.6 via this function.
void KeyboardScreen_keyboard(char key, bool pressed, int index)
{
    // no need any action when bluetooth keyboard is not connected
    if (!bleKeyboard.isConnected())
        return;

    // take the index and find the mapping key
    key = _usb_keyboard_layers[0][index];

    // check if lower or raise is click
    static int layer = 0;

    // LOWER is pressed
    if (_usb_keyboard_layers[0][index] == KEY_F24)
    {
        if (pressed)
            layer = 1;
        else
            layer = 0;

        // layer key does not count a key press
        // ignore the layer key press
        return;
    }

    // RAISE is clicked
    if (_usb_keyboard_layers[0][index] == KEY_F23)
    {
        if (pressed)
            layer = 2;
        else
            layer = 0;

        // layer key does not count a key press
        // ignore the layer key press
        return;
    }

    //
    key = _usb_keyboard_layers[layer][index];
    _debug("[KeyboardScreen_keyboard] layer %d index %d key %d\n", layer, index, key);

    // ignore dead keys
    if (key == 0)
        return;

    // MENU button is pressed
    if (key == MENU)
    {
        // move over the text
        KeyboardScreen_copy();
        return;
    }

    //
    /////////////
    if (pressed)
        bleKeyboard.press(key);
    else
        bleKeyboard.release(key);
}

void KeyboardScreen_copy()
{
    File file = gfs()->open(Editor::getInstance().fileName.c_str(), "r");
    if (!file || !file.available())
    {
        _log("KeyboardScreen_copy: Failed to open file\n");
        return;
    }

    _log("Sending file content over BLE keyboard...\n");

    // Read file content in small chunks
    while (file.available())
    {
        char c = file.read();

        // If needed, convert line endings
        if (c == '\n')
        {
            bleKeyboard.write(KEY_RETURN);
        }
        else if (isPrintable(c) || c == '\t') // Avoid sending non-printable control characters
        {
            bleKeyboard.print(c);
        }

        // Small delay to prevent buffer overflow
        delay(5); // You can tweak this as needed
    }

    file.close();

    _log("KeyboardScreen_copy: File sent.\n");
}