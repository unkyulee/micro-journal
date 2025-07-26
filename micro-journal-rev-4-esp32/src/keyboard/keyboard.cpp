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

#ifdef REV6
#include "keyboard/Keypad/48/keypad_48.h"
#endif

#ifdef REV5
#include "keyboard/USBHost/USBHost.h"
#include "keyboard/Button/button.h"
#endif

#ifdef BOARD_PICO
#include <Keyboard.h>
#endif
#ifdef BOARD_ESP32_S3
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
  // button_setup();
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
  // Front Button Setup
  // button_loop();
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

void keyboard_config_load(String filename, int *layers, int size)
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
      app["screen"] = ERRORSCREEN;

      //
      _log(app["error"]);

      return;
    }

    // overwrite to layers
    // int layers[LAYERS][ROWS * COLS]
    const char *keys[] = {"main", "alt"};

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

    // Close the file
    file.close();
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
#endif

#ifdef BOARD_ESP32_S3
  else if (_hid == "PRINT_SCREEN")
    return KEY_PRTSC;
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

  // If no match, return 0
  return 0;
}