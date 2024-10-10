#ifndef Keyboard_h
#define Keyboard_h

// MENU button
#define FN 28 

// 
#define SHIFT 0x02

// special key
#define EMPTY 0x0
#define MENU 0x6

void keyboard_setup();
void keyboard_loop();

// Tracking backspace
int keyboard_backspace_last();
void keyboard_backspace_last_set(int last);

bool keyboard_backspace_pressed();
void keyboard_backspace_pressed_set(bool pressed);

// capslock
bool keyboard_capslock();
void keyboard_capslock_toggle();

// numlock
bool keyboard_numlock();
void keyboard_numlock_toggle();

#endif