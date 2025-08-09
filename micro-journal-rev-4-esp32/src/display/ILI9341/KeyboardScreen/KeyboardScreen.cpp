#include "KeyboardScreen.h"
#include "app/app.h"
#include "display/display.h"
#include "service/Editor/Editor.h"
#include "keyboard/keyboard.h"

//
#include <NimBLEDevice.h>
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
     KEY_ESC, 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '\b',
     '\t', 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'',
     KEY_LEFT_SHIFT, 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', KEY_RIGHT_SHIFT,
     KEY_LEFT_CTRL, KEY_LEFT_GUI, KEY_LEFT_ALT, KEY_F23, KEY_F24, ' ', ' ',  KEY_LEFT_ARROW, KEY_DOWN_ARROW, KEY_UP_ARROW, KEY_RIGHT_ARROW, '\n'},

     {// lower
     KEY_ESC, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', KEY_DELETE,
     '`', 'a', 's', 'd', 'f', 'g', 'h', 'j', '-', '=', '[', ']',
     KEY_LEFT_SHIFT, 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', KEY_RIGHT_SHIFT,
     KEY_LEFT_CTRL, KEY_LEFT_GUI, KEY_LEFT_ALT, KEY_F23, KEY_F24, ' ', ' ',  KEY_HOME, KEY_PAGE_DOWN, KEY_PAGE_UP, KEY_END, '\n'},

    {// raise
     KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6, KEY_F7, KEY_F8, KEY_F9, KEY_F10, KEY_F11, KEY_F12,
     '\\', 'a', 's', 'd', 'f', 'g', 'h', 'j', '-', '=', '[', ']',
     KEY_LEFT_SHIFT, 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', KEY_PRTSC,
     KEY_LEFT_CTRL, KEY_LEFT_GUI, KEY_LEFT_ALT, KEY_F23, KEY_F24, ' ', ' ',  KEY_HOME, KEY_PAGE_DOWN, KEY_PAGE_UP, KEY_END, '\n'},

    
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

    // Only redraw screen when connection status changes
    if (keyboardConnected != keyboardConnectedPrev)
    {
        ptft->fillScreen(TFT_BLACK);
        keyboardConnectedPrev = keyboardConnected;

        // Screen dimensions
        int screenW = ptft->width();
        int screenH = ptft->height();

        // Title
        ptft->setTextSize(2);
        ptft->setTextColor(TFT_WHITE, TFT_BLACK);
        ptft->setCursor(20, 10);
        ptft->println("BLUETOOTH KEYBOARD");

        // Status Box
        ptft->setTextSize(2);
        ptft->setCursor(45, 60);
        if (keyboardConnected)
        {
            ptft->setTextColor(TFT_GREEN, TFT_BLACK);
            ptft->print("Status: Connected");
        }
        else
        {
            ptft->setTextColor(TFT_YELLOW, TFT_BLACK);
            ptft->print("Status: Waiting...");
        }

        // Instruction Box
        ptft->drawRoundRect(20, 120, screenW - 40, 100, 8, TFT_WHITE);
        ptft->setTextSize(1);
        ptft->setTextColor(TFT_CYAN, TFT_BLACK);
        ptft->setCursor(40, 140);
        ptft->println("Turn off device to end session.");
        ptft->setCursor(40, 160);
        ptft->println("Press top-left and top-right keys");
        ptft->setCursor(40, 180);
        ptft->println("simultaneously to SEND.");
    }
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
    static int send = 0;

    // check if first and the last key of the first row is both clicked
    if (index == 0)
    {
        if (pressed)
            send += 1;
        else
            send = 0;
    }
    else if (index == 11)
    {
        if (pressed)
            send += 1;
        else
            send = 0;
    }

    // if send is activated
    if (send == 2)
    {
        // move over the text
        send = 0;
        KeyboardScreen_copy();
        return;
    }

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
        delay(15); // You can tweak this as needed
    }

    file.close();

    _log("KeyboardScreen_copy: File sent.\n");
}