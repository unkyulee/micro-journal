#include "KeyboardScreen.h"
#include "app/app.h"
#include "display/display.h"

//
#include "../display_EPD.h"

//
#include <BleKeyboard.h>

BleKeyboard bleKeyboard;

//
void KeyboardScreen_setup()
{
    _log("Keyboard Screen Setup\n");
    // Clear Screen
    epd_poweron();
    epd_clear_quick(epd_full_screen(), 4, 50);

    // Text to be displayed
    JsonDocument &app = status();
    String text = "Bluetooth Keyboard Mode.";

    // Turn on the display
    epd_poweron();

    // Draw text
    int32_t x = 18;
    int32_t y = 50;
    write_string((GFXfont *)&systemFont, text.c_str(), &x, &y, NULL);

    // Turn off the display
    // epd_poweroff_all();
    epd_poweroff();

    // Setup Bluetooth Keyboard
    bleKeyboard.setName("M.J. Rev.7");
    bleKeyboard.begin();
    _log("Bluetooth Keyboard Started\n");
}

//
void KeyboardScreen_render()
{
    bleKeyboard.isConnected();
}

void KeyboardScreen_keyboard(uint8_t modifier, uint8_t reserved, uint8_t *keycodes)
{
    if (!bleKeyboard.isConnected())
        return;

    // send out the scan code
    KeyReport reportCopy;
    reportCopy.modifiers = modifier;
    reportCopy.reserved = reserved;
    for (int i = 0; i < 6; i++)
        reportCopy.keys[i] = keycodes[i];

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