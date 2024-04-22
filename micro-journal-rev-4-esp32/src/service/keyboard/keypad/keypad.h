#ifdef KEYPAD
#ifndef Keyboard_Keypad_h
#define Keyboard_Keypad_h

//
#include <Adafruit_Keypad.h>

#define FN 28
#define SHIFT 0x02

// special key
#define EMPTY 0x0
#define MENU 0x6

void keyboard_setup();

///
void keyboard_loop();

// 
int keyboard_get_key(keypadEvent e);
void keyboard_key(char key);

#endif
#endif