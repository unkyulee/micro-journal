#include "keyboard.h"
#include "app/app.h"

#ifdef KEYPAD_68
#include "service/InputMethod/Keypad_68/keypad_68.h"
#endif

//
void keyboard_setup()
{
#ifdef KEYPAD_68
  keyboard_keypad_68_setup();
#endif
}

//
void keyboard_loop()
{
#ifdef KEYPAD_68
  keyboard_keypad_68_loop();
#endif
}



/*
// TBD: tracking press and release and trigger multiple times until released
int _backspace_last = 0;
int keyboard_backspace_last()
{
  return _backspace_last;
}

void keyboard_backspace_last_set(int last)
{
  _backspace_last = last;
}

// backspaace pressed
bool _backspace_pressed = false;
bool keyboard_backspace_pressed()
{
  return _backspace_pressed;
}
void keyboard_backspace_pressed_set(bool pressed)
{
  _backspace_pressed = pressed;
}


// capslock
bool _capslock = false;
bool keyboard_capslock()
{
  return _capslock;
}

void keyboard_capslock_toggle()
{
  _capslock = !_capslock;
}

// numlock
bool _numlock = false;
bool keyboard_numlock()
{
  return _numlock;
}

void keyboard_numlock_toggle()
{
  _numlock = !_numlock;
}
*/