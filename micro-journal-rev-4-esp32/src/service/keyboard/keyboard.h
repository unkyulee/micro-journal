#ifndef Keyboard_h
#define Keyboard_h

//
#include <Adafruit_Keypad.h>

//
void keyboard_setup();

///
void keyboard_loop();

// 
int keyboard_get_key(keypadEvent e);

#endif