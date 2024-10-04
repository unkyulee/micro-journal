// Handling Keyboard and Providing Interface to GUI (display)

#include "keyboard.h"
#include "app/app.h"
#include "config/config.h"
#include "display/display.h"

//
#include "display/WordProcessor/WordProcessor.h"
#include "display/ErrorScreen/ErrorScreen.h"
#include "display/Menu/Menu.h"
#include "display/WakeUp/WakeUp.h"

// Rev.5 receives USB and Bluetooth (BLE) Keyboards
#ifdef ENV_USBHOST
#include "hid/hid.h"
#endif

// Rev.6 receives Key presses directly from the keypad
#ifdef ENV_KEYBOARD
#include "keypad/keypad.h"
#endif

bool keyboard_setup_completed = false;

void keyboard_setup()
{
#ifdef ENV_USBHOST
  keyboard_hid_setup();
#endif

#ifdef ENV_KEYBOARD
  // keypad setup
  keyboard_keypad_setup();
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

#ifdef ENV_USBHOST
  keyboard_hid_loop();
#endif

#ifdef ENV_KEYBOARD
  // keypad loop
  keyboard_keypad_loop();
#endif
}

// Redirect the key press to the current GUI
void keyboard_key(char key)
{
  JsonDocument &app = app_status();
  int screen = app["screen"].as<int>();

  if (screen == WORDPROCESSOR)
  {
    // send the key stroke to word processor
    WP_keyboard(key);
  }
  else if (screen == MENUSCREEN)
  {
    Menu_keyboard(key);
  }
  else if (screen == ERRORSCREEN)
  {
    ErrorScreen_keyboard(key);
  }
  else if (screen == WAKEUPSCREEN || screen == SLEEPSCREEN)
  {
    WakeUp_keyboard(key);
  }
}
