#include "keypad_68.h"

//
#include "app/app.h"

//
#include "display/display.h"

//
#define LAYERS 4 // layers

// GPIO PINS
#define ROWS 8 // rows
byte rowPins[ROWS] = {0, 1, 2, 3, 4, 5, 6, 7};

#define COLS 9 // columns
byte colPins[COLS] = {13, 14, 15, 16, 17, 18, 19, 20, 21};

//
// KEYBOARD LAYOUT
//

// https://www.ascii-code.com/

// 2 - Home
// 3 - End

// 4 - Ctrl
// 5 - Alt
// 6 - Gui

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

// layers
// prettier-ignore
int layers[LAYERS][ROWS * COLS] = {

    {// normal layers
     27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b', 127,
     '\t', 'Q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', 22,
     7, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '\\', 23,
     14, '`', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 14, 20, 3,
     1, 7, ' ', ' ', 4, 17, 2, 18, 21, 19},

    {// when shift is pressed
     27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b', 127,
     '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', 22,
     7, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '\\', 23,
     14, '`', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 14, 20, 3,
     1, 7, 4, ' ', 4, 17, 2, 18, 21, 19},

    {// alt layer
     27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b', 127,
     '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', 22,
     7, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '\\', 23,
     14, '`', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 14, 20, 3,
     1, 7, 4, ' ', 4, 17, 2, 18, 21, 19},

    {// alt layer shift
     27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b', 127,
     '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', 22,
     7, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '\\', 23,
     14, '`', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 14, 20, 3,
     1, 7, 4, ' ', 4, 17, 2, 18, 21, 19},

};

// define the symbols on the buttons of the keypads
// prettier-ignore
char keys[ROWS][COLS] = {
    {0, 1, 2, 3, 4, 5, 6, 7, 8},
    {9, 10, 11, 12, 13, 14, 15, 16, 17},
    {18, 19, 20, 21, 22, 23, 24, 25, 26},
    {27, 28, 29, 30, 31, 32, 33, 34, 35},
    {36, 37, 38, 39, 40, 41, 42, 43, 44},
    {45, 46, 47, 48, 49, 50, 51, 52, 53},
    {54, 55, 56, 57, 58, 59, 60, 61, 62},
    {63, 64, 65, 66, 67, 68, 69, 70, 71}    
}; 
    

Adafruit_Keypad customKeypad = Adafruit_Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

//
int layer = 0;
bool _shift_pressed = false;
bool _fn_pressed = false;

// initialize keymap
void keyboard_keypad_68_setup()
{
    //
    customKeypad.begin();

    //
    _log("Keypad initialized\n");
}

//
void keyboard_keypad_68_loop()
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
            //
            keypadEvent e = customKeypad.read();
            _debug("[keyboard_keypad_68_loop] Key: %d, Event: %d, Row: %d, Col: %d\n",
                 e.bit.KEY, e.bit.EVENT, e.bit.ROW, e.bit.COL);

            // check if the key is pressed
            int character = keyboard_keypad_68_get_key(e);
            _debug("[keyboard_keypad_68_get_key] Character: '%c' [%d]\n", character, character);

            // send over the key to the display
            display_keyboard(character, e.bit.EVENT == KEY_JUST_PRESSED);         
        }
    }
}

// Translate the key press into a key code
int keyboard_keypad_68_get_key(keypadEvent e)
{
    // define the layer
    layer = 0;

    // check if the layer key is pressed
    if (_fn_pressed)
        layer = 2;
    // check if the shift key is pressed
    if (_shift_pressed)
        layer += 1;

    // return the corresponding key
    return layers[layer][e.bit.KEY];
}
