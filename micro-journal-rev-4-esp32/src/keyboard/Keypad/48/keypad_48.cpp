#include "keypad_48.h"
#include "../Keypad.h"

//
#include "app/app.h"

//
#include "display/display.h"

//
#define LAYERS 4 // layers
#define ROWS 4   // rows
#define COLS 12  // columns

// 2 - Home
// 3 - End

// 14 - SHIFT
// 17 - LAYER

// 18 - Left
// 19 - Right
// 20 - Up
// 21 - Down

// 22 - Page Up
// 23 - Page Down

// 26 - CAPSLOCK
// 27 - ESC

// 127 - DEL

// layers
// prettier-ignore
int layers[LAYERS][ROWS * COLS] = {

    {// normal layers
     '`', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', ',',
     27, 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', 8,
     17, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', '\'', '\n',
     14, 'z', 'x', 'c', 'v', ' ', 'b', 'n', 'm', '.', '/', 14},

    {// when shift is pressed
     '~', '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '<',
     27, 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', 127,
     17, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', '\"', '\n',
     14, 'Z', 'X', 'C', 'V', ' ', 'B', 'N', 'M', '>', '?', 14},

    {// alt layer
     '`', '1', '2', '3', '4', '5', '6', '7', '8', '[', ']', '\\',
     27, 'q', 'w', 'e', 'r', 't', 22, 'u', 20, '-', '=', 127, // 2, 20, 3, 22,
     17, 'a', 's', 'd', 'f', 'g', 23, 18, 21, 19, ';', '\n',  //  18, 21, 19, 23
     14, 'z', 'x', 'c', 'v', ' ', 'b', 2, 'm', 3, '/', 14},

    {// alt layer shift
     '~', '!', '@', '#', '$', '%', '^', '&', '*', '{', '}', '|',
     27, 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', '_', '+', 127,
     17, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '\n',
     14, 'Z', 'X', 'C', 'V', ' ', 'B', 'N', 'M', '<', '?', 14},

};

// define the symbols on the buttons of the keypads
// prettier-ignore
char keys[ROWS][COLS] = {
    {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11},
    {12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23},
    {24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35},
    {36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47},
};

//
byte rowPins[ROWS] = {15, 16, 17, 18};
byte colPins[COLS] = {1, 2, 42, 41, 40, 39, 45, 48, 47, 21, 20, 19};

//
Adafruit_Keypad customKeypad = Adafruit_Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// initialize keymap
void keyboard_keypad_48_setup()
{
    // load keyboard.json if exists
    keypad_load_config("/keyboard.json", (int *)layers, COLS);

    //
    customKeypad.begin();

    //
    _log("Keypad initialized\n");
}

///
void keyboard_keypad_48_loop()
{
    //
    static unsigned int last = 0;
    if (millis() - last > 10)
    {
        //
        last = millis();

        // put your main code here, to run repeatedly:
        customKeypad.tick();
        while (customKeypad.available())
        {
            //
            keypadEvent e = customKeypad.read();

            //
            // check if the key is pressed
            int character = keyboard_keypad_48_get_key(e);

            // send over the key to the display
            _debug("[keyboard_keypad_48_get_key] Key: %d, Event: %d, Row: %d, Col: %d Character: [%d] '%c'\n",
                   e.bit.KEY, e.bit.EVENT, e.bit.ROW, e.bit.COL,
                   character, character);

            //
            display_keyboard(character, e.bit.EVENT == KEY_JUST_PRESSED, e.bit.KEY);
        }
    }
}

//
// Translate the key press into a key code
//
int _layer = 0;
bool _shift_pressed = false;
bool _fn_pressed = false;
//
int keyboard_keypad_48_get_key(keypadEvent e)
{
    // define the layer
    _layer = 0;

    //
    int key = layers[_layer][e.bit.KEY];
    if (key == 17)
    {
        if (e.bit.EVENT == KEY_JUST_PRESSED)
        {
            _fn_pressed = true;
            return 0;
        }
        else
        {
            _fn_pressed = false;
            return 0;
        }
    }
    else if (key == 14)
    {
        if (e.bit.EVENT == KEY_JUST_PRESSED)
        {
            _shift_pressed = true;
            return 0;
        }

        else if (e.bit.EVENT == KEY_JUST_RELEASED)
        {
            _shift_pressed = false;
            return 0;
        }
    }

    // check if the layer key is pressed
    if (_fn_pressed)
        _layer = 2;
    // check if the shift key is pressed
    if (_shift_pressed)
        _layer += 1;

    // return the corresponding key
    return layers[_layer][e.bit.KEY];
}
