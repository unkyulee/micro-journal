#include "KeyboardScreen.h"
#include "app/app.h"
#include "display/display.h"
#include "service/Editor/Editor.h"

//
#include <BleKeyboard.h>

BleKeyboard bleKeyboard;
bool keyboardConnectedPrev = false;
//
void KeyboardScreen_setup(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    ptft->fillScreen(TFT_BLACK);

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

    ptft->println("Turn off the device to end the writing session");
    ptft->println("Pressing Left Knob will send the text");
}

void KeyboardScreen_keyboard(char key, bool pressed, int index)
{
    if (!bleKeyboard.isConnected())
        return;

    // ignore non printable characters
    if (key == 0)
        return;

    // MENU button is pressed
    if (key == 27)
    {
        // move over the text
        KeyboardScreen_copy();
        return;
    }

    // cursor key
    switch (key)
    {
    case 18:
        key = KEY_LEFT_ARROW;
        break;
    case 19:
        key = KEY_RIGHT_ARROW;
        break;
    case 20:
        key = KEY_UP_ARROW;
        break;
    case 21:
        key = KEY_DOWN_ARROW;
        break;
    case 22:
        key = KEY_PAGE_UP;
        break;
    case 23:
        key = KEY_PAGE_DOWN;
        break;
    case 2:
        key = KEY_HOME;
        break;
    case 3:
        key = KEY_END;
        break;
    case 127:
        key = KEY_DELETE;
        break;
    }
    //
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