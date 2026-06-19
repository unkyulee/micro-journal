#include "keypad_68.h"
#include "../Keypad.h"

//
#include "app/app.h"

//
#include "display/display.h"

//
#include "keyboard/Locale/locale.h"

//
#define LAYERS 4 // layers
#define COLS 9   // columns
#define ROWS 8   // rows

// HID keycode (USB HID Usage Tables, Keyboard/Keypad page) for each physical
// key, matching the layout of layers[0]. 0 marks keys that aren't translated
// through the locale tables (control keys, Fn/Shift, MENU, etc).
// prettier-ignore
uint8_t key_hid[ROWS * COLS] = {
    0,    0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x2d, 0x2e, 0,    0,
    0x2c, 0x14, 0x1a, 0x08, 0x15, 0x17, 0x1c, 0x18, 0x0c, 0x12, 0x13, 0x2f, 0x30, 0,    0,
    0,    0x04, 0x16, 0x07, 0x09, 0x0a, 0x0b, 0x0d, 0x0e, 0x0f, 0x33, 0x34, 0x31, 0,
    0,    0x35, 0x1d, 0x1b, 0x06, 0x19, 0x05, 0x11, 0x10, 0x36, 0x37, 0x38, 0,    0,    0,
    0,    0,    0,    0x2c, 0,    0,    0,    0,    0,    0,
    0,
};

// layers
// prettier-ignore
int layers[LAYERS][ROWS * COLS] = {

    {// normal layers
     27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b', 127,
     ' ', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', 22,
     0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '\\', 23,
     14, '`', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 14, 20, 3,
     0, 0, 17, ' ', 17, 17, 2, 18, 21, 19,
     MENU},

    {// when shift is pressed
     27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b', 127,
     ' ', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n', 22,
     0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '|', 23,
     14, '~', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 14, 20, 3,
     0, 0, 17, ' ', 17, 17, 2, 18, 21, 19,
     MENU},

    {// alt layer
     27, 1001, 1002, 1003, 1004, 1005, 1006, 1007, 1008, 1009, 1000, '-', '=', '\b', 127,
     ' ', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', 22,
     0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '\\', 23,
     14, '`', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 14, 20, 3,
     0, 0, 17, ' ', 17, 17, 2, 18, 21, 19,
     MENU},

    {// alt layer shift
     27, 1001, 1002, 1003, 1004, 1005, 1006, 1007, 1008, 1009, 1000, '-', '=', '\b', 127,
     ' ', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n', 22,
     0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '|', 23,
     14, '~', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 14, 20, 3,
     0, 0, 17, ' ', 17, 17, 2, 18, 21, 19,
     MENU},

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


#ifdef BOARD_PICO
byte rowPins[ROWS] = {0, 1, 2, 3, 4, 5, 6, 7};
byte colPins[COLS] = {13, 14, 15, 16, 17, 18, 19, 20, 21};
#endif

#ifdef BOARD_ESP32_S3
byte rowPins[ROWS] = {8, 18, 17, 16, 15, 7, 6, 5};
byte colPins[COLS] = {1, 2, 42, 41, 40, 39, 45, 48, 47};
#endif

//
Adafruit_Keypad customKeypad = Adafruit_Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// initialize keymap
void keyboard_keypad_68_setup()
{
    // load keyboard.json if exists
    const char *keys[] = {"main", "main-shift", "alt", "alt-shift"};
    keypad_load_config("/keyboard.json", (int *)layers, ROWS * COLS, keys, 4);

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
            // Check if knob is long pressed
            // Detect the knob click by the position of the key index
            if (e.bit.KEY == 69 || e.bit.KEY == 0)
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
            _debug("[keyboard_keypad_68_loop] Key: %d, Event: %d, Row: %d, Col: %d Character: [%d]\n",
                   e.bit.KEY, e.bit.EVENT, e.bit.ROW, e.bit.COL, character);
            display_keyboard(character, e.bit.EVENT == KEY_JUST_PRESSED, e.bit.KEY);
        }
    }

#ifdef USE_SERIAL_KEYBOARD
    // receive SERIAL input and redirect to the display
    if (Serial.available())
    {
        char c = Serial.read();
        /*
                // simulate up, down, left, right
                if (c == 'w')
                    c = 20;
                if (c == 'a')
                    c = 18;
                if (c == 's')
                    c = 21;
                if (c == 'd')
                    c = 19;
        */
        if (c == 13)
            return; // ignore /r key
        _debug("Serial keyboard input %c %d\n", c, c);

        // find the scan code based on the input received

        // You can choose a key index, e.g., 0 for generic input
        display_keyboard(c, true);  // Key press
        display_keyboard(c, false); // Key release (optional, for GUI consistency)
    }
#endif
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

    // value baked into the layer tables (hardcoded to the US layout)
    key = layers[_layer][e.bit.KEY];

    // when a non-US layout is configured, re-map character keys through
    // the locale tables instead of returning the hardcoded US character
    JsonDocument &app = status();
    String locale = app["config"]["keyboard_layout"].as<String>();
    if (locale.length() > 0 && locale != "US")
    {
        uint8_t hid = key_hid[e.bit.KEY];
        if (hid != 0)
        {
            uint8_t ascii = keyboard_keycode_ascii(locale, hid, _shift_pressed, _fn_pressed, e.bit.EVENT == KEY_JUST_PRESSED);
            if (ascii != 0)
                key = ascii;
        }
    }

    // return the corresponding key
    return key;
}
