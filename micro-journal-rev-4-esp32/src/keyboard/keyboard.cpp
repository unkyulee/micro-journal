#include "keyboard.h"
#include "app/app.h"
#include "display/display.h"

#ifdef KEYPAD_68
#include "keyboard/Keypad/68/keypad_68.h"
#include "keyboard/Knob/Knob.h"
#endif

#ifdef REV7
#include "keyboard/USBHost/USBHost.h"
#endif

//
void keyboard_setup()
{
#ifdef REV7
  // setup USB Host
  USBHost_setup();
#endif

#ifdef KEYPAD_68
  keyboard_keypad_68_setup();
  knob_setup();
#endif
}

//
void keyboard_loop()
{
#ifdef REV7
  USBHost_loop();
#endif

#ifdef KEYPAD_68
  keyboard_keypad_68_loop();
  knob_loop();
#endif
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