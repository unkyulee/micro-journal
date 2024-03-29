#include "keyboard.h"
#include "app/app.h"
#include "app/network/network.h"
#include "service/display/display.h"

//
#include "screens/WordProcessor/WordProcessor.h"
#include "screens/ErrorScreen/ErrorScreen.h"
#include "screens/Menu/Menu.h"

//
#define LAYERS 4 // layers
#define ROWS 3   // rows
#define COLS 10  // columns

//
// KEYBOARD LAYOUT
//
// layers
// prettier-ignore
int layers[LAYERS][ROWS * COLS] = {
    {// normal layers
     'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p',
     'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', '\n',
     'z', 'x', 'c', 'v', ' ', 'b', 'n', 'm', FN, SHIFT},
    {// when shift is pressed
     'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P',
     'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', '\n',
     'Z', 'X', 'C', 'V', ' ', 'B', 'N', 'M', FN, SHIFT},
    {// when number layer key is pressed
     '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
     '`', '\\', '-', '=', '[', ']', ';', '\'', EMPTY, '\n',
     MENU, EMPTY, EMPTY, EMPTY, BACKSPACE, ',', '.', '/', FN, SHIFT},
    {// when number layer key and shift is pressed
     '!', '@', '#', '$', '%', '^', '&', '*', '(', ')',
     '~', '|', '_', '+', '{', '}', ':', '\"', EMPTY, '\n',
     MENU, EMPTY, EMPTY, EMPTY, BACKSPACE, '<', '>', '?', FN, SHIFT}};

// define the symbols on the buttons of the keypads
// prettier-ignore
char keys[COLS][ROWS] = {
    {20, 10, 0},
    {21, 11, 1},
    {22, 12, 2},
    {23, 13, 3},
    {24, 14, 4},
    {25, 15, 5},
    {26, 16, 6},
    {27, 17, 7},
    {28, 18, 8},
    {29, 19, 9}};

// connect to the row pinouts of the keypad
byte rowPins[ROWS] = {32, 33, 25};
// connect to the column pinouts of the keypad
byte colPins[COLS] = {
    26, 27, 14, 12, 13,
    15, 2, 4, 16, 17};

Adafruit_Keypad customKeypad = Adafruit_Keypad(makeKeymap(keys), colPins, rowPins, COLS, ROWS);

// initialize keymap
void keyboard_setup()
{
    customKeypad.begin();
}

///
void keyboard_loop()
{
    static unsigned int last = 0;
    if (millis() - last > 10)
    {
        //
        last = millis();

        // put your main code here, to run repeatedly:
        customKeypad.tick();

        while (customKeypad.available())
        {
            keypadEvent e = customKeypad.read();
            int key = keyboard_get_key(e);
            if (key != EMPTY)
            {
                // depending on the screen
                // send the keystrokes
                JsonDocument &app = app_status();
                int screen = app["screen"].as<int>();

                if(screen == WORDPROCESSOR) {
                    // send the key stroke to word processor
                    WP_keyboard((char)key);
                } else if(screen == MENUSCREEN) {
                    Menu_keyboard((char)key);
                } else if(screen == ERRORSCREEN) {
                    ErrorScreen_keyboard((char)key);
                }
            }
        }
    }
}

bool _shift_pressed = false;
bool _fn_pressed = false;
int keyboard_get_key(keypadEvent e)
{
    //
    // step 1. layer processing
    //
    if (e.bit.KEY == FN)
    {
        if (e.bit.EVENT == KEY_JUST_PRESSED)
        {
            _fn_pressed = true;
        }
        else
        {
            _fn_pressed = false;
        }
    }
    else if (e.bit.KEY == SHIFT)
    {
        if (e.bit.EVENT == KEY_JUST_PRESSED)
        {
            _shift_pressed = true;
        }

        else if (e.bit.EVENT == KEY_JUST_RELEASED)
        {
            _shift_pressed = false;
        }
    }

    // stop here
    if (e.bit.KEY == SHIFT || e.bit.KEY == FN)
    {
        return EMPTY;
    }

    // step 2. process the key
    if (e.bit.EVENT == KEY_JUST_PRESSED)
    {
        // define the layer
        int layer = 0;

        // check if the layer key is pressed
        if (_fn_pressed)
            layer = 2;
        // check if the shift key is pressed
        if (_shift_pressed)
            layer += 1;

        // return the corresponding key
        return layers[layer][e.bit.KEY];
    }

    // Serial.printf("KEY: %d EVENT: %d\n", e.bit.KEY, e.bit.EVENT);
    return EMPTY;
}