#include "KeyboardScreen.h"
#include "app/app.h"
#include "display/display.h"
#include "service/Editor/Editor.h"
#include "keyboard/keyboard.h"

//
#include "service/Editor/Editor.h"
#include "service/Send/Send.h"

//
#include <NimBLEDevice.h>
#include <BleKeyboard.h>

//
#include "display/CARDPUTER/display_CARDPUTER.h"

BleKeyboard bleKeyboard;
int keyboardConnectedPrev = -1;

// function reference used for SEND
void _send_key(int key)
{
    _debug("_send_key: %d\n", key);

    // If needed, convert line endings
    char c = (char)key;
    if (c == '\n')
    {
        bleKeyboard.press('\n');
        delay(5);
        bleKeyboard.release('\n');
    }
    else if (isPrintable(c) || c == '\t') // Avoid sending non-printable control characters
    {
        bleKeyboard.press(c);
        delay(5);
        bleKeyboard.release(c);
    }

    // BLE requires delay per key press to be stable
    delay(10);
}

//
void KeyboardScreen_setup()
{
    // clear screen
    M5Cardputer.Display.fillRect(
        0,
        0,
        M5Cardputer.Display.width(),
        M5Cardputer.Display.height(),
        TFT_BLACK);

    // register send function
    send_register(_send_key);

    // reset flags
    keyboardConnectedPrev = -1;

    // Setup Bluetooth Keyboard
    bleKeyboard.setName("MJ CardPuter");
    bleKeyboard.begin();
    _log("Bluetooth Keyboard Started\n");
}

//
void KeyboardScreen_render()
{
    bool keyboardConnected = bleKeyboard.isConnected();

    // Only redraw screen when connection status changes
    if (keyboardConnected != keyboardConnectedPrev)
    {

        M5Cardputer.Display.fillRect(
            0,
            0,
            M5Cardputer.Display.width(),
            M5Cardputer.Display.height(),
            TFT_BLACK);

        //
        keyboardConnectedPrev = keyboardConnected;

        // Screen dimensions
        int screenW = M5Cardputer.Display.width();
        int screenH = M5Cardputer.Display.height();

        // Title
        M5Cardputer.Display.setTextSize(0.5);
        M5Cardputer.Display.setFont(&fonts::Orbitron_Light_24);
        M5Cardputer.Display.setTextColor(TFT_WHITE, TFT_BLACK);

        M5Cardputer.Display.drawString("BLUETOOTH KEYBOARD", 5, 5);

        // Status Box
        if (keyboardConnected)
        {
            M5Cardputer.Display.setTextColor(TFT_GREEN, TFT_BLACK);
            M5Cardputer.Display.drawString("Status: Connected", 5, 30);
        }
        else
        {
            M5Cardputer.Display.setTextColor(TFT_YELLOW, TFT_BLACK);
            M5Cardputer.Display.drawString("Status: Waiting...", 5, 30);
        }

        // Instruction Box
        M5Cardputer.Display.setTextColor(TFT_CYAN, TFT_BLACK);
        drawWrappedText(
            "Turn off device to end session. Press FN + Tab keys to SEND",
            5, 50, M5Cardputer.Display.width());
    }
}

// keyboard message will come from Rev.6 via this function.
void KeyboardScreen_keyboard(int key, bool pressed, int index)
{
    // no need any action when bluetooth keyboard is not connected
    if (!bleKeyboard.isConnected())
        return;

    // any key pressed is going to stop sending text
    JsonDocument &app = status();
    app["send_stop"] = true;

    // check if first and the last key of the first row is both clicked
    // if send is activated
    if (key == MENU)
    {
        // request SEND to background task
        app["task"] = "send_start";
        app["send_stop"] = false;
        app["send_finished"] = false;

        return;
    }

    //
    _debug("[KeyboardScreen_keyboard] index %d key %d\n",index, key);

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
