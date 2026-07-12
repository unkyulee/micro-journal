#include "keypad_48.h"
#include "../Keypad.h"

//
#include "app/app.h"

//
#include "display/display.h"

//
#include "keyboard/Locale/locale.h"
#include "keyboard/Locale/ko/ko.h"

//
#define LAYERS 6 // layers

#if defined(BOARD_ESP32_S3)
//
#define ROWS 4  // rows
#define COLS 12 // columns

//
byte rowPins[ROWS] = {15, 16, 17, 18};
byte colPins[COLS] = {1, 2, 42, 41, 40, 39, 45, 48, 47, 21, 20, 19};


// 2 - Home
// 3 - End

// 14 - SHIFT

// 17 - LOWER
// 24 - RAISE

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
     MENU, 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '\b',
     '\\', 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'',
     14, 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 14,
     '-', '=', '[', ']', 17, ' ', ' ', 18, 21, 20, 19, '\n'},

    {// when shift is pressed
     MENU, 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', 127,
     '|', 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"',
     14, 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 14,
     '_', '+', '{', '}', 17, ' ', ' ', 2, 21, 20, 3, '\n'},

    {// LOWER layer
     MENU, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', 127,
     '`', 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'',
     14, 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 14,
     '-', '=', '[', ']', 17, ' ', ' ', 2, 21, 20, 3, '\n'},

    {// LOWER layer shift
     MENU, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', 127,
     '~', 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '\'',
     14, 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 14,
     '_', '+', '{', '}', 17, ' ', ' ', 2, 21, 20, 3, '\n'},

     {// RAISE layer
     MENU, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', 127,
     '`', 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'',
     14, 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 14,
     '-', '=', '[', ']', 17, ' ', ' ', 2, 21, 20, 3, '\n'},

    {// RAISE layer shift
     MENU, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', 127,
     '~', 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '\'',
     14, 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 14,
     '_', '+', '{', '}', 17, ' ', ' ', 2, 21, 20, 3, '\n'},

};

// define the symbols on the buttons of the keypads
// prettier-ignore
char keys[ROWS][COLS] = {
    {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11},
    {12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23},
    {24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35},
    {36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47},
};

// HID keycode (USB HID Usage Tables) for each physical key, aligned to
// layers[0]. Only the letter keys are populated - they are the ones the
// locale tables remap (a Korean layout turns them into jamo); control and
// punctuation keys stay 0 and keep their hardcoded layer value.
// prettier-ignore
uint8_t key_hid[ROWS * COLS] = {
    0,    0x14, 0x1a, 0x08, 0x15, 0x17, 0x1c, 0x18, 0x0c, 0x12, 0x13, 0,    // MENU q w e r t y u i o p bksp
    0,    0x04, 0x16, 0x07, 0x09, 0x0a, 0x0b, 0x0d, 0x0e, 0x0f, 0,    0,    // \ a s d f g h j k l ; '
    0,    0x1d, 0x1b, 0x06, 0x19, 0x05, 0x11, 0x10, 0,    0,    0,    0,    // shift z x c v b n m , . / shift
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,     // - = [ ] lower spc spc left down up right enter
};

#endif

#if defined(BOARD_PICO)
//
#define ROWS 5  // rows
#define COLS 12 // columns0-

//
byte rowPins[ROWS] = {0, 1, 2, 3, 21};
byte colPins[COLS] = {4, 5, 6, 7, 13, 14, 15, 16, 17, 18, 19, 20};

// layers
// prettier-ignore
int layers[LAYERS][ROWS * COLS] = {

    {// normal layers
     27, 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '\b',
     '\\', 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'',
     14, 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 14,
     '-', '=', '[', ']', 17, ' ', ' ', 18, 21, 20, 19, '\n',
     MENU},

    {// when shift is pressed
     27, 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', 127,
     '|', 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"',
     14, 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 14,
     '_', '+', '{', '}', 17, ' ', ' ', 2, 21, 20, 3, '\n',
     MENU},

    {// LOWER layer
     27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', 127,
     '`', 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'',
     14, 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 14,
     '-', '=', '[', ']', 17, ' ', ' ', 2, 21, 20, 3, '\n',
     MENU},

    {// LOWER layer shift100923455678901---00
     27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', 127,
     '~', 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '\'',
     14, 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 14,
     '_', '+', '{', '}', 17, ' ', ' ', 2, 21, 20, 3, '\n',
     MENU},

     {// RAISE layer
     27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', 127,
     '`', 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'',
     14, 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 14,
     '-', '=', '[', ']', 17, ' ', ' ', 2, 21, 20, 3, '\n',
     MENU},

    {// RAISE layer shift
     27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', 127,
     '~', 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '\'',
     14, 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 14,
     '_', '+', '{', '}', 17, ' ', ' ', 2, 21, 20, 3, '\n',
     MENU},

};

// define the symbols on the buttons of the keypads
// prettier-ignore
char keys[ROWS][COLS] = {
    {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11},
    {12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23},
    {24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35},
    {36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47},
    {48},
};

#endif



//
Adafruit_Keypad customKeypad = Adafruit_Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// initialize keymap
void keyboard_keypad_48_setup()
{
    _debug("[keyboard_keypad_48_setup]\n");

    // load keyboard.json if exists
    const char *keys[] = {"main", "main-shift", "lower", "lower-shift", "raise", "raise-shift"};
    keypad_load_config("/keyboard.json", (int *)layers, COLS * ROWS, keys, 6);

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
bool _lower_pressed = false;
bool _raise_pressed = false;

//
int keyboard_keypad_48_get_key(keypadEvent e)
{
    // define the layer
    _layer = 0;

    //
    int key = layers[_layer][e.bit.KEY];
    if (key == 17) // LOWER
    {
        if (e.bit.EVENT == KEY_JUST_PRESSED)
        {
            _lower_pressed = true;
            return 0;
        }
        else
        {
            _lower_pressed = false;
            return 0;
        }
    }
    else if (key == 24) // RAISE
    {
        if (e.bit.EVENT == KEY_JUST_PRESSED)
        {
            _raise_pressed = true;
            return 0;
        }
        else
        {
            _raise_pressed = false;
            return 0;
        }
    }
    else if (key == 14) // SHIFT
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
    if (_lower_pressed)
        _layer = 2;
    if(_raise_pressed)
        _layer = 4;
    // check if the shift key is pressed
    if (_shift_pressed)
        _layer += 1;

    // value baked into the layer tables (hardcoded to the US layout)
    key = layers[_layer][e.bit.KEY];

    JsonDocument &app = status();
    String locale = app["config"]["keyboard_layout"].as<String>();

    // Korean 한/영 toggle: LOWER + Space (this keypad has no dedicated Alt
    // key). Routed through the locale's toggle path (alt + keycode 0),
    // which flips Hangul/English and types nothing. Gated to KR so LOWER +
    // Space stays a normal space under every other layout.
    if (locale == "KR" && _lower_pressed && key == ' ')
    {
        return keyboard_keycode_ascii(locale, 0, _shift_pressed, true, e.bit.EVENT == KEY_JUST_PRESSED);
    }

    // Korean maps by the CHARACTER the layer table produced, not by the
    // physical key: 'r' -> ㄱ, 'R' -> ㄲ, 'c' -> ㅊ ... This keypad is
    // small and layouts are commonly customized via keyboard.json, so a
    // physical-key (HID) mapping would break the moment a letter moves
    // to a different key - the resolved character is the source of truth.
    if (locale == "KR")
    {
        key = keyboard_ascii_ko(key, e.bit.EVENT == KEY_JUST_PRESSED);
    }

    // for other non-US layouts, re-map character keys through the locale
    // tables instead of returning the hardcoded US character (see
    // Keypad_68 for the same pattern)
    else if (locale.length() > 0 && locale != "US" && locale != "null")
    {
        uint8_t hid = key_hid[e.bit.KEY];
        if (hid != 0)
        {
            int ascii = keyboard_keycode_ascii(locale, hid, _shift_pressed, _raise_pressed, e.bit.EVENT == KEY_JUST_PRESSED);
            if (ascii != 0)
                key = ascii;
        }
    }

    // return the corresponding key
    return key;
}
