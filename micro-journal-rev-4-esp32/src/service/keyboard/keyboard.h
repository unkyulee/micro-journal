#ifndef Keyboard_h
#define Keyboard_h

//
#include <Adafruit_Keypad.h>

#define FN 28
#define SHIFT 29

// special key
#define EMPTY 0x0
#define MENU 0x6
#define BACKSPACE 0x8

//
void keyboard_setup();

///
void keyboard_loop();

// 
int keyboard_get_key(keypadEvent e);

#endif