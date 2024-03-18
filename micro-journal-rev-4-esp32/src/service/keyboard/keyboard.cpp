#include "keyboard.h"
#include "app/app.h"
#include "app/network/network.h"

//
#define LAYERS 4 // layers
#define ROWS 3   // rows
#define COLS 10  // columns

// define the symbols on the buttons of the keypads
char keys[ROWS][COLS] = {
    {0, 1, 2, 3, 4, 5, 6, 7, 8, 9},
    {10, 11, 12, 13, 14, 15, 16, 17, 18, 19},
    {20, 21, 22, 23, 24, 25, 26, 27, 28, 29}};

byte rowPins[ROWS] = {16, 17, 5};                              // connect to the row pinouts of the keypad
byte colPins[COLS] = {13, 12, 14, 27, 26, 25, 33, 32, 21, 22}; // connect to the column pinouts of the keypad

Adafruit_Keypad customKeypad = Adafruit_Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// layers
// prettier-ignore
int layers[LAYERS][ROWS * COLS] = {
    {// normal layers
     'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p',
     'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', '\n',
     'z', 'x', 'c', 'v', ' ', 'b', 'n', 'm', -1, 1},
    {// when shift is pressed
     'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P',
     'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', '\n',
     'Z', 'X', 'C', 'V', ' ', 'B', 'N', 'M', -1, 1},
    {// when number layer key is pressed
     '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
     '`', '\\', 0, '-', '=', '[', ']', ';', '\'', '\n',
     0, 0, 0, 0, ' ', ',', '.', '/', -1, 1},
    {// when number layer key is pressed
     '!', '@', '#', '$', '%', '^', '&', '*', '(', ')',
     '~', '|', 0, '_', '+', '{', '}', ':', '\"', '\n',
     0, 0, 0, 0, ' ', '<', '>', '?', -1, 1}};

void keyboard_setup_main()
{
    customKeypad.begin();
    network_enable(true);
}
void keyboard_setup_secondary() {}

///
void keyboard_loop_main()
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
            if (key > 1)
            {
                Serial.print((char)key);
                if (e.bit.EVENT == KEY_JUST_PRESSED)
                    Serial.println(" pressed");
                else if (e.bit.EVENT == KEY_JUST_RELEASED)
                    Serial.println(" released");
            }
        }
    }
}
void keyboard_loop_secondary() {}

int keyboard_get_key(keypadEvent e)
{
    // only handle event when key is pressed
    if (e.bit.EVENT != KEY_JUST_PRESSED)
        return 0;

    // define the layer
    int layer = 0;

    // check if the layer key is pressed
    if (e.bit.KEY == -1)
    {
        layer = 2;
    }

    // if SHIFT is pressed increase the layer by one
    if (e.bit.KEY == 1)
    {
        layer += 1;
    }

    return layers[layer][e.bit.KEY];
}