#include "keyboard.h"
#include "app/app.h"
#include "display/display.h"

//
#include "keyboard/Locale/locale.h"

#ifdef KEYPAD_68
#include "keyboard/Keypad/68/keypad_68.h"
#include "keyboard/Knob/Knob.h"
#endif

#ifdef REV7
#include "keyboard/USBHost/USBHost.h"
#endif

#ifdef REV6
#include "keyboard/Keypad/48/keypad_48.h"
#endif

#ifdef REV5
#include "keyboard/USBHost/USBHost.h"
#include "keyboard/Button/button.h"
#include "keyboard/BLE/ble.h"
#endif

#ifdef BOARD_PICO
#include <Keyboard.h>
#endif

#if defined(BOARD_ESP32_S3)
#include <BleKeyboard.h>
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

#ifdef REV5
  // setup USB Host
  USBHost_setup();

  // Front Button Setup
  button_setup();
#endif

#ifdef REV6
  keyboard_keypad_48_setup();
#endif
}

//
void keyboard_loop()
{
#ifdef REV7
  USBHost_loop();
#endif

#ifdef REV6
  keyboard_keypad_48_loop();
#endif

#ifdef REV5
  // setup USB Host
  USBHost_loop();

  // setup BLE Keyboard
  ble_loop();

  // Front Button Setup
  button_loop();
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

void keyboard_config_load(
    String filename,
    int *layers,
    int size,
    const char *keys[],
    int keyCount)
{
  //
  JsonDocument &app = status();

  // check if file exists in SD card
  if (gfs()->exists(filename.c_str()))
  {
    _log("Loading %s from file system\n", filename.c_str());
    // load image
    File file = gfs()->open(filename.c_str(), "r");
    if (!file)
    {
      _log("Failed to open %s for reading\n", filename.c_str());
      return;
    }

    //
    String fileString = file.readString();
    file.close();
    delay(100);

    //
    _debug(fileString.c_str());
    _debug("\n");

    //
    // Prepare a JsonDocument for the keyboard configuration
    // The size should be adjusted according to your configuration's needs
    JsonDocument keyboardConfig;
    // convert to JsonObject
    DeserializationError error = deserializeJson(keyboardConfig, fileString);
    if (error)
    {
      //
      app["error"] = format("%s not in a correct json form: %s\n", filename.c_str(), error.c_str());
      // app["screen"] = ERRORSCREEN;
      // app["screen_next"] = KEYBOARDSCREEN;

      //
      _log(app["error"]);

      return;
    }

    // save the current key pos
    int pos = 0;
    for (int i = 0; i < 4; i++)
    {
      const char *key = keys[i];
      JsonArray keyArray = keyboardConfig[key].as<JsonArray>();

      int pos = 0;
      for (JsonVariant obj : keyArray)
      {
        //
        String key = obj.as<String>();
        int _hid = keyboard_convert_HID(key);
        if (_hid != 0)
        {
          *(layers + i * size + pos++) = _hid;
        }

        else if (key.length() == 1)
        {
          // Use ASCII value of the single character
          *(layers + i * size + pos++) = (uint8_t)key.charAt(0);
        }
        else
        {
          // Unsupported key or invalid string, store 0 or handle accordingly
          *(layers + i * size + pos++) = 0;
        }
      }
    }

    // load macro
    for (int i = 0; i < 10; i++)
    {
      String key = format("MACRO_%d", i);
      if (keyboardConfig[key].is<const char *>())
      {
        // load macro
        app[key] = keyboardConfig[key].as<const char *>();
        _debug("Loading Macro %s - %s\n", key.c_str(), app[key].as<const char *>());
      }
    }
  }
}

int keyboard_convert_HID(String _hid)
{
  // Modifiers
  if (_hid == "LEFT_CTRL")
    return KEY_LEFT_CTRL;
  else if (_hid == "LEFT_SHIFT")
    return KEY_LEFT_SHIFT;
  else if (_hid == "LEFT_ALT")
    return KEY_LEFT_ALT;
  else if (_hid == "LEFT_GUI")
    return KEY_LEFT_GUI;
  else if (_hid == "RIGHT_CTRL")
    return KEY_RIGHT_CTRL;
  else if (_hid == "RIGHT_SHIFT")
    return KEY_RIGHT_SHIFT;
  else if (_hid == "RIGHT_ALT")
    return KEY_RIGHT_ALT;
  else if (_hid == "RIGHT_GUI")
    return KEY_RIGHT_GUI;

  // Misc keys
  else if (_hid == "UP_ARROW")
    return KEY_UP_ARROW;
  else if (_hid == "DOWN_ARROW")
    return KEY_DOWN_ARROW;
  else if (_hid == "LEFT_ARROW")
    return KEY_LEFT_ARROW;
  else if (_hid == "RIGHT_ARROW")
    return KEY_RIGHT_ARROW;
  else if (_hid == "BACKSPACE")
    return KEY_BACKSPACE;
  else if (_hid == "TAB")
    return KEY_TAB;
  else if (_hid == "RETURN")
    return KEY_RETURN;
  else if (_hid == "ESC")
    return KEY_ESC;
  else if (_hid == "INSERT")
    return KEY_INSERT;
  else if (_hid == "DELETE")
    return KEY_DELETE;
  else if (_hid == "PAGE_UP")
    return KEY_PAGE_UP;
  else if (_hid == "PAGE_DOWN")
    return KEY_PAGE_DOWN;
  else if (_hid == "HOME")
    return KEY_HOME;
  else if (_hid == "END")
    return KEY_END;
  else if (_hid == "CAPS_LOCK")
    return KEY_CAPS_LOCK;

#ifdef BOARD_PICO
  else if (_hid == "PRINT_SCREEN")
    return KEY_PRINT_SCREEN;
  else if (_hid == "SCROLL_LOCK")
    return KEY_SCROLL_LOCK;
  else if (_hid == "PAUSE")
    return KEY_PAUSE;
  // Numeric keypad
  else if (_hid == "NUM_LOCK")
    return KEY_NUM_LOCK;
  else if (_hid == "SLASH")
    return KEY_KP_SLASH;
  else if (_hid == "ASTERISK")
    return KEY_KP_ASTERISK;
  else if (_hid == "MINUS")
    return KEY_KP_MINUS;
  else if (_hid == "PLUS")
    return KEY_KP_PLUS;
  else if (_hid == "ENTER")
    return KEY_KP_ENTER;
  else if (_hid == "KP_1")
    return KEY_KP_1;
  else if (_hid == "KP_2")
    return KEY_KP_2;
  else if (_hid == "KP_3")
    return KEY_KP_3;
  else if (_hid == "KP_4")
    return KEY_KP_4;
  else if (_hid == "KP_5")
    return KEY_KP_5;
  else if (_hid == "KP_6")
    return KEY_KP_6;
  else if (_hid == "KP_7")
    return KEY_KP_7;
  else if (_hid == "KP_8")
    return KEY_KP_8;
  else if (_hid == "KP_9")
    return KEY_KP_9;
  else if (_hid == "KP_0")
    return KEY_KP_0;
  else if (_hid == "DOT")
    return KEY_KP_DOT;
#endif

#if defined(BOARD_ESP32_S3)
  else if (_hid == "PRINT_SCREEN")
    return KEY_PRTSC;
  // Numeric keypad
  else if (_hid == "SLASH")
    return KEY_NUM_SLASH;
  else if (_hid == "ASTERISK")
    return KEY_NUM_ASTERISK;
  else if (_hid == "MINUS")
    return KEY_NUM_MINUS;
  else if (_hid == "PLUS")
    return KEY_NUM_PLUS;
  else if (_hid == "ENTER")
    return KEY_NUM_ENTER;
  else if (_hid == "KP_1")
    return KEY_NUM_1;
  else if (_hid == "KP_2")
    return KEY_NUM_2;
  else if (_hid == "KP_3")
    return KEY_NUM_3;
  else if (_hid == "KP_4")
    return KEY_NUM_4;
  else if (_hid == "KP_5")
    return KEY_NUM_5;
  else if (_hid == "KP_6")
    return KEY_NUM_6;
  else if (_hid == "KP_7")
    return KEY_NUM_7;
  else if (_hid == "KP_8")
    return KEY_NUM_8;
  else if (_hid == "KP_9")
    return KEY_NUM_9;
  else if (_hid == "KP_0")
    return KEY_NUM_0;
  else if (_hid == "DOT")
    return KEY_NUM_PERIOD;
#endif

  // Function keys
  else if (_hid == "F1")
    return KEY_F1;
  else if (_hid == "F2")
    return KEY_F2;
  else if (_hid == "F3")
    return KEY_F3;
  else if (_hid == "F4")
    return KEY_F4;
  else if (_hid == "F5")
    return KEY_F5;
  else if (_hid == "F6")
    return KEY_F6;
  else if (_hid == "F7")
    return KEY_F7;
  else if (_hid == "F8")
    return KEY_F8;
  else if (_hid == "F9")
    return KEY_F9;
  else if (_hid == "F10")
    return KEY_F10;
  else if (_hid == "F11")
    return KEY_F11;
  else if (_hid == "F12")
    return KEY_F12;

  // LAYER key is assigned to F24
  else if (_hid == "LAYER")
    return KEY_F24;

  // LAYER key is assigned to F24
  else if (_hid == "LOWER")
    return KEY_F24;

  // LAYER key is assigned to F24
  else if (_hid == "RAISE")
    return KEY_F23;

  // LAYER key is assigned to F24
  else if (_hid == "MENU")
    return MENU;

  // MACRO
  else if (_hid == "MACRO_0")
    return 2000;
  else if (_hid == "MACRO_1")
    return 2001;
  else if (_hid == "MACRO_2")
    return 2002;
  else if (_hid == "MACRO_3")
    return 2003;
  else if (_hid == "MACRO_4")
    return 2004;
  else if (_hid == "MACRO_5")
    return 2005;
  else if (_hid == "MACRO_6")
    return 2006;
  else if (_hid == "MACRO_7")
    return 2007;
  else if (_hid == "MACRO_8")
    return 2008;
  else if (_hid == "MACRO_9")
    return 2009;

  // If no match, return 0
  return 0;
}

//
void keyboard_HID2Ascii(uint8_t keycode, uint8_t modifier, bool pressed)
{
  //////////////////////////////////////////
  // CAPSLOCK is pressed
  // 0x39 - HID_KEY_CAPS_LOCK
  if (keycode == 0x39 && pressed)
  {
    // Mark caps lock status
    keyboard_capslock_toggle();
    return;
  }

  //////////////////////////////////////////
  // MENU
  // ESC key is MENU button
  if (keycode == 0x29)
  {
    display_keyboard(MENU, pressed, 69);
    return;
  }

  //////////////////////////////////////////
  // General Key Press
  // Check SHIFT key pressed
  // TU_BIT(1) - (1UL << (1))
  // KEYBOARD_MODIFIER_LEFTCTRL   = TU_BIT(0), ///< Left Control
  // KEYBOARD_MODIFIER_LEFTSHIFT  = TU_BIT(1), ///< Left Shift
  // KEYBOARD_MODIFIER_LEFTALT    = TU_BIT(2), ///< Left Alt
  // KEYBOARD_MODIFIER_LEFTGUI    = TU_BIT(3), ///< Left Window
  // KEYBOARD_MODIFIER_RIGHTCTRL  = TU_BIT(4), ///< Right Control
  // KEYBOARD_MODIFIER_RIGHTSHIFT = TU_BIT(5), ///< Right Shift
  // KEYBOARD_MODIFIER_RIGHTALT   = TU_BIT(6), ///< Right Alt
  // KEYBOARD_MODIFIER_RIGHTGUI   = TU_BIT(7)  ///< Right Window
  bool shift = (modifier & (1UL << (1))) || (modifier & (1UL << (5)));

  // Check ALT key pressed
  bool alt = (modifier & (1UL << (2))) || (modifier & (1UL << (6)));

  // Translate the Keycode to ASCII
  JsonDocument &app = status();
  String locale = app["config"]["keyboard_layout"].as<String>();
  uint8_t ascii = keyboard_keycode_ascii(locale, keycode, shift, alt, pressed);
  if (ascii != 0)
    // send key to GUI
    display_keyboard(ascii, pressed, keycode);
}