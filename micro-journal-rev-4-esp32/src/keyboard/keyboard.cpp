#include "keyboard.h"
#include "app/app.h"
#include "display/display.h"

#ifdef KEYPAD_68
#include "keyboard/InputMethod/Keypad/68/keypad_68.h"
#include "keyboard/InputMethod/Knob/Knob.h"
#endif

//
void keyboard_setup()
{
#ifdef KEYPAD_68
  keyboard_keypad_68_setup();
  knob_setup();
#endif
}

//
void keyboard_loop()
{
#ifdef KEYPAD_68
  keyboard_keypad_68_loop();
  knob_loop();
#endif
}
