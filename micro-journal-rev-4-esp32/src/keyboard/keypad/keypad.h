#ifndef Keyboard_Keypad_h
#define Keyboard_Keypad_h

//
#include <Adafruit_Keypad.h>

void keyboard_keypad_setup();

///
void keyboard_keypad_loop();

// 
int keyboard_get_key(keypadEvent e);

#endif