// Handling Keyboard and Providing Interface to GUI (display)

#include "keyboard.h"
#include "app/app.h"
#include "config/config.h"
#include "display/display.h"

#ifdef ENV_KEYBOARD
// Rev.6 receives Key presses directly from the keypad
#include "keypad/keypad.h"
#else
// Rev.5 and Rev.7 receives USB and Bluetooth (BLE) Keyboards
#include "hid/hid.h"
#endif

bool keyboard_setup_completed = false;

void keyboard_setup()
{
 
#ifdef ENV_KEYBOARD
  // keypad setup
  keyboard_keypad_setup();
#else
  keyboard_hid_setup();
#endif

  // Keyboard setup is completed
  // Keyboard loop can initiate
  keyboard_setup_completed = true;
}

void keyboard_loop()
{
  // Wait until the keyboard setup is completed
  if (!keyboard_setup_completed)
    return;

#ifdef ENV_KEYBOARD
  // keypad loop
  keyboard_keypad_loop();
#else
  keyboard_hid_loop();
#endif

}

//
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
