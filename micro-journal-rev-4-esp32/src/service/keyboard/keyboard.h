#ifndef Keyboard_h
#define Keyboard_h

//
#include <Adafruit_Keypad.h>

//
void keyboard_setup_main();
void keyboard_setup_secondary();

///
void keyboard_loop_main();
void keyboard_loop_secondary();

// 
int keyboard_get_key(keypadEvent e);

#endif