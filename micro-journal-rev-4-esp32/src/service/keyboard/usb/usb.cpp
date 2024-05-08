#include "usb.h"
#include "app/app.h"
#include "app/config/config.h"
#include "service/display/display.h"

//
#include "screens/WordProcessor/WordProcessor.h"
#include "screens/ErrorScreen/ErrorScreen.h"
#include "screens/Menu/Menu.h"
#include "screens/WakeUp/WakeUp.h"

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

class MyEspUsbHost : public EspUsbHost
{
  //
  uint8_t getKeycodeToAscii(uint8_t keycode, uint8_t shift, uint8_t altgr)
  {
    //
    static uint8_t const keyboard_conv_table_us[128][2] = {HID_KEYCODE_TO_ASCII_US};
    static uint8_t const keyboard_conv_table_it[128][3] = {HID_KEYCODE_TO_ASCII_IT};
    static uint8_t const keyboard_conv_table_ca_bi[128][5] = {HID_KEYCODE_TO_ASCII_CA_BI};

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
        shift = 2;

      // Italian
      return keyboard_conv_table_it[keycode][shift];
    }

    //
    // Canadian Bilingual
    //
    else if (hidLocal == HID_LOCAL_Canadian_Bilingual)
    {
      if (altgr > 0)
        shift = 2;

      // Canadian Bilingual
      return keyboard_conv_table_ca_bi[keycode][shift];
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
      if (ascii == '~' || ascii == '`' || ascii == '"' || ascii == '^')
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
      if (pre_cursor_ascii != 0)
      {
        // ~  a n o
        uint8_t found = 0;
        if (pre_cursor_ascii == '~' && ascii == 'a')
        {
          found = 227; // ã
        }
        else if (pre_cursor_ascii == '~' && ascii == 'A')
        {
          found = 195; // Ã
        }
        else if (pre_cursor_ascii == '~' && ascii == 'n')
        {
          found = 241; // ñ
        }
        else if (pre_cursor_ascii == '~' && ascii == 'N')
        {
          found = 209; // Ñ
        }
        else if (pre_cursor_ascii == '~' && ascii == 'o')
        {
          found = 245; // õ
        }
        else if (pre_cursor_ascii == '~' && ascii == 'O')
        {
          found = 213; // Õ
        }

        // ` a e i o u
        else if (pre_cursor_ascii == '`' && ascii == 'a')
        {
          found = 224; // à
        }
        else if (pre_cursor_ascii == '`' && ascii == 'A')
        {
          found = 192; // À
        }
        else if (pre_cursor_ascii == '`' && ascii == 'e')
        {
          found = 232; // è
        }
        else if (pre_cursor_ascii == '`' && ascii == 'E')
        {
          found = 200; // È
        }
        else if (pre_cursor_ascii == '`' && ascii == 'i')
        {
          found = 236; // ì
        }
        else if (pre_cursor_ascii == '`' && ascii == 'I')
        {
          found = 204; // Ì
        }
        else if (pre_cursor_ascii == '`' && ascii == 'o')
        {
          found = 242; // ò
        }
        else if (pre_cursor_ascii == '`' && ascii == 'O')
        {
          found = 210; // Ò
        }
        else if (pre_cursor_ascii == '`' && ascii == 'u')
        {
          found = 249; // ù
        }
        else if (pre_cursor_ascii == '`' && ascii == 'U')
        {
          found = 217; // Ù
        }

        // " a e i o u y
        else if (pre_cursor_ascii == '\"' && ascii == 'a')
        {
          found = 228; // ä
        }
        else if (pre_cursor_ascii == '\"' && ascii == 'A')
        {
          found = 196; // Ä
        }
        else if (pre_cursor_ascii == '\"' && ascii == 'e')
        {
          found = 235; // ë
        }
        else if (pre_cursor_ascii == '\"' && ascii == 'E')
        {
          found = 203; // Ë
        }
        else if (pre_cursor_ascii == '\"' && ascii == 'i')
        {
          found = 239; // ï
        }
        else if (pre_cursor_ascii == '\"' && ascii == 'I')
        {
          found = 207; // Ï
        }
        else if (pre_cursor_ascii == '\"' && ascii == 'o')
        {
          found = 246; // ö
        }
        else if (pre_cursor_ascii == '\"' && ascii == 'O')
        {
          found = 214; // Ö
        }
        else if (pre_cursor_ascii == '\"' && ascii == 'u')
        {
          found = 252; // ü
        }
        else if (pre_cursor_ascii == '\"' && ascii == 'U')
        {
          found = 220; // Ü
        }
        else if (pre_cursor_ascii == '\"' && ascii == 'y')
        {
          found = 255; // ÿ
        }

        // ^ a e i o u
        else if (pre_cursor_ascii == '^' && ascii == 'a')
        {
          found = 226; // â
        }
        else if (pre_cursor_ascii == '^' && ascii == 'A')
        {
          found = 194; // Â
        }
        else if (pre_cursor_ascii == '^' && ascii == 'e')
        {
          found = 234; // ê
        }
        else if (pre_cursor_ascii == '^' && ascii == 'E')
        {
          found = 202; // Ê
        }
        else if (pre_cursor_ascii == '^' && ascii == 'i')
        {
          found = 238; // î
        }
        else if (pre_cursor_ascii == '^' && ascii == 'I')
        {
          found = 206; // Î
        }
        else if (pre_cursor_ascii == '^' && ascii == 'o')
        {
          found = 244; // ô
        }
        else if (pre_cursor_ascii == '^' && ascii == 'O')
        {
          found = 212; // Ô
        }
        else if (pre_cursor_ascii == '^' && ascii == 'u')
        {
          found = 251; // û
        }
        else if (pre_cursor_ascii == '^' && ascii == 'U')
        {
          found = 219; // Û
        }

        //
        else
        {
          // precursor invalid just send out the key press
          //
          onKeyboardKey(pre_cursor_ascii, 0, 0);

          // then clear the precursor and send out the key stroke
          pre_cursor_ascii = 0;

          // if space is pressed then don't print
          if (ascii == ' ')
            return 0;
        }

        // type latin character
        if (found > 0)
        {
          // reset precursor
          pre_cursor_ascii = 0;

          //
          onKeyboardKey(found, 0, 0);

          //
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

    if (ascii == 27)
    {
      keyboard_key(MENU);
    }

    else if (ascii != 0)
    {
      keyboard_key(ascii);
    }
  };

  void onKeyboard(hid_keyboard_report_t report, hid_keyboard_report_t last_report)
  {
    // app_log("%02x %02x %02x %02x %02x %02x\n", report.modifier, report.keycode[0], report.keycode[1], report.keycode[2], report.keycode[3], report.keycode[4], report.keycode[5]);
  }
};

MyEspUsbHost usbHost;
bool mass_drive_mode = false;

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
  else if (layout == "IT")
  {
    usbHost.setHIDLocal(HID_LOCAL_Italian);
  }
  else if (layout == "CA")
  {
    usbHost.setHIDLocal(HID_LOCAL_Canadian_Bilingual);
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

void keyboard_setup()
{
  JsonDocument &app = app_status();
  if (app["config"]["drive"].as<bool>())
  {
    //
    mass_drive_mode = true;
    app_log("Loaded as mass drive. USB Host disabled.\n");
  }

  else
  {
    //
    app_log("Init Keyboard\n");

    // usb host setup
    usbHost.begin();

    // update the locale depending on your keyboard layout
    //
    String layout = app["config"]["keyboard_layout"].as<String>();
    if (layout == "null" || layout.isEmpty())
      layout = "US"; // defaults to US layout

    keyboard_layout(layout);
  }
}

///
void keyboard_loop()
{
  // when booted as mass storage then skip usb host
  if (mass_drive_mode)
    return;

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
}

void keyboard_key(char key)
{
  //
  // depending on the screen
  // send the keystrokes
  JsonDocument &app = app_status();
  int screen = app["screen"].as<int>();

  if (screen == WORDPROCESSOR)
  {
    // send the key stroke to word processor
    WordProcessor::getInstance(nullptr, nullptr).keyboard(key);
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
