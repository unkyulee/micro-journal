#include "usb.h"
#include "app/app.h"
#include "config/config.h"
#include "keyboard/keyboard.h"
#include "display/display.h"
#include "keyboard/ascii/ascii.h"

// display button handler
#include "keyboard/usb/button/button.h"

//
#include <EspUsbHost.h>

/*
  HID_LOCAL_NotSupported = 0   , ///< NotSupported
  HID_LOCAL_Arabic             , ///< Arabic
  HID_LOCAL_Belgian            , ///< Belgian
  HID_LOCAL_Canadian_Bilingual , ///< Canadian_Bilingual
  HID_LOCAL_Canadian_French    , ///< Canadian_French
  HID_LOCAL_Czech_Republic     , ///< Czech_Republic
  HID_LOCAL_Danish             , ///< Danish
  HID_LOCAL_Finnish            , ///< Finnish
  HID_LOCAL_French             , ///< French
  HID_LOCAL_German             , ///< German
  HID_LOCAL_Greek              , ///< Greek
  HID_LOCAL_Hebrew             , ///< Hebrew
  HID_LOCAL_Hungary            , ///< Hungary
  HID_LOCAL_International      , ///< International
  HID_LOCAL_Italian            , ///< Italian
  HID_LOCAL_Japan_Katakana     , ///< Japan_Katakana
  HID_LOCAL_Korean             , ///< Korean
  HID_LOCAL_Latin_American     , ///< Latin_American
  HID_LOCAL_Netherlands_Dutch  , ///< Netherlands/Dutch
  HID_LOCAL_Norwegian          , ///< Norwegian
  HID_LOCAL_Persian_Farsi      , ///< Persian (Farsi)
  HID_LOCAL_Poland             , ///< Poland
  HID_LOCAL_Portuguese         , ///< Portuguese
  HID_LOCAL_Russia             , ///< Russia
  HID_LOCAL_Slovakia           , ///< Slovakia
  HID_LOCAL_Spanish            , ///< Spanish
  HID_LOCAL_Swedish            , ///< Swedish
  HID_LOCAL_Swiss_French       , ///< Swiss/French
  HID_LOCAL_Swiss_German       , ///< Swiss/German
  HID_LOCAL_Switzerland        , ///< Switzerland
  HID_LOCAL_Taiwan             , ///< Taiwan
  HID_LOCAL_Turkish_Q          , ///< Turkish-Q
  HID_LOCAL_UK                 , ///< UK
  HID_LOCAL_US                 , ///< US
  HID_LOCAL_Yugoslavia         , ///< Yugoslavia
  HID_LOCAL_Turkish_F            ///< Turkish-F
*/

#include "locale/ca-bi.h"
#include "locale/it.h"
#include "locale/us.h"
#include "locale/ge.h"
#include "locale/be.h"
#include "locale/uk.h"

// back space
int _usb_backspace_last = 0;
bool _usb_backspace_pressed = false;

class MyEspUsbHost : public EspUsbHost
{
  //
  uint8_t getKeycodeToAscii(uint8_t keycode, uint8_t shift, uint8_t altgr, bool released)
  {
    // left - 80 right - 79 up - 82 down - 81 home - 74 end - 77

    //
    static uint8_t const keyboard_conv_table_us[128][2] = {HID_KEYCODE_TO_ASCII_US};
    static uint8_t const keyboard_conv_table_it[128][3] = {HID_KEYCODE_TO_ASCII_IT};
    static uint8_t const keyboard_conv_table_ca_bi[128][5] = {HID_KEYCODE_TO_ASCII_CA_BI};
    static uint8_t const keyboard_conv_table_ge[128][3] = {HID_KEYCODE_TO_ASCII_GE};
    static uint8_t const keyboard_conv_table_be[128][3] = {HID_KEYCODE_TO_ASCII_BE};
    static uint8_t const keyboard_conv_table_uk[128][2] = {HID_KEYCODE_TO_ASCII_UK};

    //
    static uint8_t pre_cursor_ascii = 0;

    // debug
    // app_log("%d %d %d\n", keycode, shift, altgr);

    //
    if (shift > 1)
      shift = 1;

    //
    // Italian Keyboard
    //
    if (hidLocal == HID_LOCAL_Italian)
    {
      if (altgr > 0)
        shift += 2;

      // Italian
      return keyboard_conv_table_it[keycode][shift];
    }

    //
    // Belgium
    //
    else if (hidLocal == HID_LOCAL_Belgian)
    {
      if (altgr > 0)
        shift += 2;

      // international layout
      uint8_t ascii = keyboard_conv_table_be[keycode][shift];

      //
      // when precursor keys are typed
      if (released == false)
      {
        if (ascii == '~' || ascii == '`' || ascii == '"' || ascii == '\'' || ascii == '^')
        {
          // check if previous precursor is set
          if (pre_cursor_ascii != 0)
          {
            //
            onKeyboardKey(pre_cursor_ascii, 0, 0);

            // then clear the precursor and send out the key stroke
            pre_cursor_ascii = 0;
          }
          else
          {
            // save to the precursor
            pre_cursor_ascii = ascii;
            // do not process this key press
            return 0;
          }
        }

        // when precursor exists
        if (pre_cursor_ascii != 0 && ascii != 0)
        {
          uint8_t found = ascii_international(pre_cursor_ascii, ascii);

          // type latin character
          if (found > 0)
          {
            // reset precursor
            pre_cursor_ascii = 0;

            // á
            onKeyboardKey(found, 0, 0);

            //
            return 0;
          }
          else
          {
            //
            // precursor invalid just send out the key press
            //
            onKeyboardKey(pre_cursor_ascii, 0, 0);

            // then clear the precursor and send out the key stroke
            pre_cursor_ascii = 0;

            // if space is pressed then don't print
            if (ascii == ' ')
              return 0;
          }
        }
      }

      return keyboard_conv_table_be[keycode][shift];
    }

    //
    // UK
    //
    else if (hidLocal == HID_LOCAL_UK)
    {
      return keyboard_conv_table_uk[keycode][shift];
    }

    //
    // Canadian Bilingual
    //
    else if (hidLocal == HID_LOCAL_Canadian_Bilingual)
    {
      if (altgr > 0)
        shift += 2;

      // Canadian Bilingual
      return keyboard_conv_table_ca_bi[keycode][shift];
    }

    //
    // HID_LOCAL_German
    //
    else if (hidLocal == HID_LOCAL_German)
    {
      // Serial.println(keycode, HEX);

      if (altgr > 0)
        shift += 2;

      // German
      return keyboard_conv_table_ge[keycode][shift];
    }

    //
    // International
    //
    else if (hidLocal == HID_LOCAL_International)
    {
      // international layout
      uint8_t ascii = keyboard_conv_table_us[keycode][shift];

      //
      // when precursor keys are typed
      if (ascii == '~' || ascii == '`' || ascii == '"' || ascii == '\'' || ascii == '^')
      {
        // check if previous precursor is set
        if (pre_cursor_ascii != 0)
        {
          //
          onKeyboardKey(pre_cursor_ascii, 0, 0);

          // then clear the precursor and send out the key stroke
          pre_cursor_ascii = 0;
        }
        else
        {
          // save to the precursor
          pre_cursor_ascii = ascii;
          // do not process this key press
          return 0;
        }
      }

      // when precursor exists
      if (pre_cursor_ascii != 0 && ascii != 0)
      {
        uint8_t found = ascii_international(pre_cursor_ascii, ascii);

        // type latin character
        if (found > 0)
        {
          // reset precursor
          pre_cursor_ascii = 0;

          // á
          onKeyboardKey(found, 0, 0);

          //
          return 0;
        }
        else
        {
          //
          // precursor invalid just send out the key press
          //
          onKeyboardKey(pre_cursor_ascii, 0, 0);

          // then clear the precursor and send out the key stroke
          pre_cursor_ascii = 0;

          // if space is pressed then don't print
          if (ascii == ' ')
            return 0;
        }
      }

      // rest of the keys uses US keyboard
      return keyboard_conv_table_us[keycode][shift];
    }

    //
    // US Layout
    //
    else
    {
      // US
      return keyboard_conv_table_us[keycode][shift];
    }

    // do nothing
    return 0;
  }

  //
  void onKeyboardKey(uint8_t ascii, uint8_t keycode, uint8_t modifier)
  {
    // release back space when any other keys are pressed
    if (_usb_backspace_pressed)
    {
      _usb_backspace_pressed = false;
    }

    // when backspace is pressed
    if (ascii == '\b')
    {
      _usb_backspace_last = millis() + 500;
      _usb_backspace_pressed = true;
    }

    if (ascii == 27)
    {
      keyboard_key(MENU);
    }

    else if (ascii != 0)
    {
      keyboard_key(ascii);
    }
  };

  void onKeyboardKeyReleased(uint8_t ascii, uint8_t keycode, uint8_t modifier)
  {
    //
    // when backspace is pressed
    if (_usb_backspace_pressed)
    {
      _usb_backspace_pressed = false;
    }
  }

  void onKeyboard(hid_keyboard_report_t report, hid_keyboard_report_t last_report)
  {

    app_log("%02x %02x %02x %02x %02x %02x %02x\n",
            report.modifier,
            report.reserved,
            report.keycode[0],
            report.keycode[1],
            report.keycode[2],
            report.keycode[3],
            report.keycode[4],
            report.keycode[5]);
  }

public:
  uint8_t publicGetKeycodeToAscii(uint8_t keycode, uint8_t shift, uint8_t altgr, bool released)
  {
    return this->getKeycodeToAscii(keycode, shift, altgr, released);
  }

public:
  void publicOnKeyboardKey(uint8_t ascii, uint8_t keycode, uint8_t modifier)
  {
    return this->onKeyboardKey(ascii, keycode, modifier);
  }
};

//
MyEspUsbHost usbHost;

uint8_t getKeycodeToAscii(uint8_t keycode, uint8_t shift, uint8_t altgr, bool released)
{
  return usbHost.publicGetKeycodeToAscii(keycode, shift, altgr, released);
}
void onKeyboardKey(uint8_t ascii, uint8_t keycode, uint8_t modifier)
{
  return usbHost.publicOnKeyboardKey(ascii, keycode, modifier);
}

// initialize USB HOST
String keyboard_layout_prev;
void keyboard_layout(String layout)
{
  app_log("Setting keyboard layout: %s\n", layout.c_str());

  // setup initial layout
  if (layout == "US")
  {
    usbHost.setHIDLocal(HID_LOCAL_US);
  }
  else if (layout == "BE")
  {
    usbHost.setHIDLocal(HID_LOCAL_Belgian);
  }
  else if (layout == "IT")
  {
    usbHost.setHIDLocal(HID_LOCAL_Italian);
  }
  else if (layout == "GE")
  {
    usbHost.setHIDLocal(HID_LOCAL_German);
  }
  else if (layout == "CA")
  {
    usbHost.setHIDLocal(HID_LOCAL_Canadian_Bilingual);
  }
  else if (layout == "UK")
  {
    usbHost.setHIDLocal(HID_LOCAL_UK);
  }
  else if (layout == "INT")
  {
    usbHost.setHIDLocal(HID_LOCAL_International);
  }
  else
  {
    usbHost.setHIDLocal(HID_LOCAL_US);
  }

  //
  keyboard_layout_prev = layout;
}

void keyboard_usb_setup()
{
  //
  JsonDocument &app = app_status();

  //
  app_log("Init USB Keyboard\n");

  // usb host setup
  usbHost.begin();

  // update the locale depending on your keyboard layout
  //
  String layout = app["config"]["keyboard_layout"].as<String>();
  if (layout == "null" || layout.isEmpty())
    layout = "US"; // defaults to US layout

  keyboard_layout(layout);

  // setup display button
  button_setup();
}

///
void keyboard_usb_loop()
{
  // as a usb host
  usbHost.task();

  static unsigned int last = 0;
  if (millis() - last > 1000)
  {
    //
    last = millis();

    // check if layout is changed
    JsonDocument &app = app_status();
    String layout = app["config"]["keyboard_layout"].as<String>();
    if (!layout.equals(keyboard_layout_prev))
    {
      app_log("Keyboard layout changed %s\n", layout);
      keyboard_layout(layout);
    }
  }

  // handle display button press
  button_loop();

  // Handle Backspace
  if (millis() > 60 + _usb_backspace_last && _usb_backspace_pressed)
  {
    _usb_backspace_last = millis();

    // send backspace key
    keyboard_key('\b');
  }
}
