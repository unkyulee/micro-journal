#include "keypad_68.h"
#include "../Keypad.h"

//
#include "app/app.h"

//
#include "display/display.h"

//
#define LAYERS 4 // layers
#define COLS 9   // columns
#define ROWS 8   // rows

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
     0x6, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b', 127,
     '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', 22,
     7, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '\\', 23,
     14, '`', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 14, 20, 3,
     0, 0, 17, ' ', 17, 0, 2, 18, 21, 19,
     0x6},

    {// when shift is pressed
     0x6, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b', 127,
     '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n', 22,
     7, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '|', 23,
     14, '~', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 14, 20, 3,
     0, 0, 17, ' ', 17, 0, 2, 18, 21, 19,
     0x6},

    {// alt layer
     0x6, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b', 127,
     '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', 22,
     7, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '\\', 23,
     14, '`', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 14, 20, 3,
     0, 0, 17, ' ', 17, 0, 2, 18, 21, 19,
     0x6},

    {// alt layer shift
     0x6, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b', 127,
     '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n', 22,
     7, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '|', 23,
     14, '~', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 14, 20, 3,
     0, 0, 17, ' ', 17, 0, 2, 18, 21, 19,
     0x6},

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
    {63, 64, 65, 66, 67, 68, 69, 70, 71}};

//
byte rowPins[ROWS] = {0, 1, 2, 3, 4, 5, 6, 7};
byte colPins[COLS] = {13, 14, 15, 16, 17, 18, 19, 20, 21};

//
Adafruit_Keypad customKeypad = Adafruit_Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// initialize keymap
void keyboard_keypad_68_setup()
{
    // load keyboard.json if exists
    keypad_load_config("/writerDeck.json", (int *)layers, COLS);

    //
    customKeypad.begin();

    // reset long press flag
    JsonDocument &app = status();
    app["knobLongPressed"] = false;

    //
    _log("Keypad initialized\n");
}

//
void keyboard_keypad_68_loop()
{
    JsonDocument &app = status();

    // knob long press logic
    static unsigned long knob_press_time = 0;
    static bool knob_pressed = false;

    // check if knob is long pressed - 2 seconds
    if (knob_pressed == true && millis() - knob_press_time > 1000 && app["knobLongPressed"].as<bool>() == false)
    {
        // knob is long pressed
        // mark the flag and send out released event        
        app["knobLongPressed"] = true;

        _debug("[keyboard_keypad_68_loop] knob long press detected\n");
        display_keyboard(MENU, KEY_JUST_RELEASED, 69);
    }

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
            _debug("[keyboard_keypad_68_loop] Key: %d, Event: %d, Row: %d, Col: %d\n",
                   e.bit.KEY, e.bit.EVENT, e.bit.ROW, e.bit.COL);

            // Check if knob is long pressed
            if (e.bit.KEY == 69)
            {
                if (e.bit.EVENT == KEY_JUST_PRESSED)
                {
                    knob_press_time = millis();
                    knob_pressed = true;
                }
                else
                {
                    knob_pressed = false;
                    // reset long press flag
                    JsonDocument &app = status();
                    if (app["knobLongPressed"].as<bool>())
                    {
                        app["knobLongPressed"] = false;

                        // do not send out a released event
                        _debug("[keyboard_keypad_68_loop] knob long pressed and ignore actual released event\n");
                        return;
                    }
                }
            }

            //
            // check if the key is pressed
            int character = keyboard_keypad_68_get_key(e);

            // send over the key to the display
            _debug("[keyboard_keypad_68_get_key] Character: '%c' [%d]\n", character, character);
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
int keyboard_keypad_68_get_key(keypadEvent e)
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
