#include "keypad.h"
#include "app/app.h"
#include "display/display.h"
//
#include "display/WordProcessor/WordProcessor.h"
#include "display/ErrorScreen/ErrorScreen.h"
#include "display/Menu/Menu.h"

//
#define LAYERS 4 // layers
#define ROWS 4   // rows
#define COLS 12  // columns

// GPIO PINS
byte rowPins[ROWS] = {42, 41, 40, 39};
byte colPins[COLS] = {38, 37, 36, 4, 5, 6, 7, 15, 16, 17, 18, 8};

//
// KEYBOARD LAYOUT
//

// https://www.ascii-code.com/
// ESC - 27
// BACKSPACE - 8
// RETURN - 13
// SHIFT - 14
// DEL - 127
// SPECIAL LAYER - 17

// layers
// prettier-ignore
int layers[LAYERS][ROWS * COLS] = {

    {// normal layers
    '`', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', ',',
    127, 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', 8,
    17, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', '\'', 13,
    14, 'z', 'x', 'c', 'v', ' ', 'b', 'n', 'm', '.', '/', 14},

    {// when shift is pressed
    '~', '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '<',
    127, 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', 8,
    9, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', '\"', 13,
    14, 'Z', 'X', 'C', 'V', ' ', 'B', 'N', 'M', '>', '?', 14},
    
    {// special layer 
    '`', '1', '2', '3', '4', '5', '6', '7', '8', '[', ']', '\\',
    127, 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', '-', '=', 8,
    17, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', 13,
    14, 'z', 'x', 'c', 'v', ' ', 'b', 'n', 'm', ',', '/', 14},

    {// special layer shift
    '~', '!', '@', '#', '$', '%', '^', '&', '*', '{', '}', '|',
    127, 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', '_', '+', 8,
    9, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', 13,
    14, 'Z', 'X', 'C', 'V', ' ', 'B', 'N', 'M', '<', '?', 14},

};

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

Adafruit_Keypad customKeypad;

// initialize keymap
void keyboard_keypad_setup()
{
    customKeypad = Adafruit_Keypad(makeKeymap(keys), colPins, rowPins, COLS, ROWS);
    customKeypad.begin();
}

///
void keyboard_keypad_loop()
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
