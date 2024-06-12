#include "keypad.h"
#include "app/app.h"
#include "display/display.h"
#include "keyboard/keyboard.h"
//
#include "display/WordProcessor/WordProcessor.h"
#include "display/ErrorScreen/ErrorScreen.h"
#include "display/Menu/Menu.h"

//
#define LAYERS 4 // layers

// GPIO PINS
#define ROWS 4 // rows
byte rowPins[ROWS] = {15, 16, 17, 18};

#define COLS 12 // columns
byte colPins[COLS] = {1, 2, 42, 41, 40, 39, 45, 48, 47, 21, 20, 19};

//
// KEYBOARD LAYOUT
//

// https://www.ascii-code.com/
// ESC - 27
// BACKSPACE - 8
// SHIFT - 14
// SPECIAL LAYER - 17

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
     27, 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', 8,
     17, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', '\"', '\n',
     14, 'Z', 'X', 'C', 'V', ' ', 'B', 'N', 'M', '>', '?', 14},

    {// special layer
     '`', '1', '2', '3', '4', '5', '6', '7', '8', '[', ']', '\\',
     27, 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', '-', '=', 8,
     17, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\n',
     14, 'z', 'x', 'c', 'v', ' ', 'b', 'n', 'm', ',', '/', 14},

    {// special layer shift
     '~', '!', '@', '#', '$', '%', '^', '&', '*', '{', '}', '|',
     27, 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', '_', '+', 8,
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

Adafruit_Keypad customKeypad = Adafruit_Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// initialize keymap
void keyboard_keypad_setup()
{
    customKeypad.begin();
    app_log("Keypad initialized\n");
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
            int ascii = keyboard_get_key(e);
            if (ascii != 0)
            {
                // depending on the screen
                // send the keystrokes
                JsonDocument &app = app_status();
                int screen = app["screen"].as<int>();

                if (ascii == 27)
                {
                    keyboard_key(MENU);
                }

                else if (ascii != 0)
                {
                    keyboard_key(ascii);
                }
            }
        }
    }
}

bool _shift_pressed = false;
bool _fn_pressed = false;
int layer = 0;
int keyboard_get_key(keypadEvent e)
{
    //
    // step 1. layer processing
    //
    int key = layers[layer][e.bit.KEY];
    if (key == 17)
    {
        if (e.bit.EVENT == KEY_JUST_PRESSED)
        {
            app_log("FN\n");
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
            app_log("Shift\n");
            _shift_pressed = true;
            return 0;
        }

        else if (e.bit.EVENT == KEY_JUST_RELEASED)
        {
            _shift_pressed = false;
            return 0;
        }
    }

    // step 2. process the key
    if (e.bit.EVENT == KEY_JUST_PRESSED)
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

    // Serial.printf("KEY: %d EVENT: %d\n", e.bit.KEY, e.bit.EVENT);
    return 0;
}
