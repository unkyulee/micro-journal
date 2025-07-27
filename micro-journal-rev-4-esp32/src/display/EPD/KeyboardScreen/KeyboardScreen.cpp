#include "KeyboardScreen.h"
#include "app/app.h"
#include "display/display.h"
#include "service/Editor/Editor.h"

//
#include "../display_EPD.h"

//
#include <BleKeyboard.h>

BleKeyboard bleKeyboard;
int keyboardConnectedPrev = -1;

//
void KeyboardScreen_setup()
{
    //
    _log("Keyboard Screen Setup\n");

    //
    keyboardConnectedPrev = -1;

    // Clear Screen
    epd_poweron();
    epd_clear_quick(epd_full_screen(), 4, 50);
    epd_poweroff();

    // Setup Bluetooth Keyboard
    bleKeyboard.setName("Micro Journal 7");
    bleKeyboard.begin();
    _log("Bluetooth Keyboard Started\n");
}

//
void KeyboardScreen_render()
{
    // clear screen if status changed
    bool keyboardConnected = bleKeyboard.isConnected();
    if (keyboardConnected != keyboardConnectedPrev)
    {
        //
        keyboardConnectedPrev = keyboardConnected;

        // render screen
    } else {
        // nothing change no need to render
        return;
    }

    // Start Rendering Screen
    epd_poweron();
    epd_clear_quick(epd_full_screen(), 4, 50);

    // Draw text
    int32_t x = 18;
    int32_t y = 50;
    write_string(display_EPD_font(), "Bluetooth Keyboard Mode", &x, &y, NULL);

    //
    x = 18;
    y += 30;
    if (keyboardConnected)
    {
        write_string((GFXfont *)&systemFont, "Keyboard Connected", &x, &y, NULL);
    } else {
        write_string((GFXfont *)&systemFont, "Waiting for connection ...", &x, &y, NULL);
    }

    x = 18;
    y += 30;
    write_string((GFXfont *)&systemFont, "Press Left Knob  will send the text", &x, &y, NULL);

    x = 18;
    y += 15;
    write_string((GFXfont *)&systemFont, "Turn off the device to end the writing session", &x, &y, NULL);
    

    //
    epd_poweroff();
}

void KeyboardScreen_keyboard(uint8_t modifier, uint8_t reserved, uint8_t *keycodes)
{
    if (!bleKeyboard.isConnected())
        return;

    // get which key is pressed
    uint8_t key = 0;
    for (int i = 0; i < 6; i++)
    {
        if (keycodes[i] != 0)
        {
            key = keycodes[i];
            break;
        }
    }

    // PAUSE key is LEFT KNOB click
    if (key == 0x48)
    {
        // move over the text
        KeyboardScreen_copy();
        return;
    }

    // send out the scan code
    KeyReport reportCopy;
    reportCopy.modifiers = modifier;
    reportCopy.reserved = reserved;
    for (int i = 0; i < 6; i++)
        reportCopy.keys[i] = keycodes[i];

    //
    bleKeyboard.sendReport(&reportCopy);
    _debug("KeyboardScreen_keyboard::%02x %02x %02x %02x %02x %02x %02x %02x\n",
           modifier,
           reserved,
           keycodes[0],
           keycodes[1],
           keycodes[2],
           keycodes[3],
           keycodes[4],
           keycodes[5]);
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