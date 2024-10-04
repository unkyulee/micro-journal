#ifndef Keyboard_h
#define Keyboard_h

// MENU button
#define FN 28 

// 
#define SHIFT 0x02

// special key
#define EMPTY 0x0
#define MENU 0x6

// Tracking backspace
int _backspace_last = 0;
bool _backspace_pressed = false;

// capslock and numlock
bool _capslock = false;
bool _numlock = false;

void keyboard_setup();
void keyboard_loop();

// sending keys to GUI
void keyboard_key(char key);

#endif