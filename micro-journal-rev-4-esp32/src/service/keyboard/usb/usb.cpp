#define USBHOST 1

#ifdef USBHOST

#include "usb.h"
#include "app/app.h"
#include "app/network/network.h"
#include "service/display/display.h"

//
#include "screens/WordProcessor/WordProcessor.h"
#include "screens/ErrorScreen/ErrorScreen.h"
#include "screens/Menu/Menu.h"

#define MENU_PIN 15
#define BACK_PIN 16

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

#include "locale/us.h"
#include "locale/it.h"

class MyEspUsbHost : public EspUsbHost
{
  //
  uint8_t getKeycodeToAscii(uint8_t keycode, uint8_t shift, uint8_t altgr)
  {
    //
    static uint8_t const keyboard_conv_table_us[128][2] = {HID_KEYCODE_TO_ASCII_US};
    static uint8_t const keyboard_conv_table_it[128][3] = {HID_KEYCODE_TO_ASCII_IT};

    // debug
    // app_log("%d %d %d\n", keycode, shift, altgr);

    //
    if (shift > 1)
      shift = 1;

    if (hidLocal == HID_LOCAL_Italian)
    {
      if (altgr > 0)
        shift = 2;

      // Italian
      return keyboard_conv_table_it[keycode][shift];
    }
    else
    {
      // US
      return keyboard_conv_table_us[keycode][shift];
    }
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
};

MyEspUsbHost usbHost;

// initialize USB HOST
void keyboard_setup()
{
  app_log("Init Keyboard\n");

  // setup two buttons on the device
  pinMode(MENU_PIN, INPUT_PULLUP);
  pinMode(BACK_PIN, INPUT_PULLUP);

  // usb host setup
  usbHost.begin();

  // update the locale depending on your keyboard layout
  // usbHost.setHIDLocal(HID_LOCAL_Italian);
}

///
void keyboard_loop()
{
  //
  usbHost.task();

  static unsigned int last = 0;
  if (millis() - last > 1000)
  {
    //
    last = millis();

    // check menu key and back key press
    int menuKey = digitalRead(MENU_PIN);
    int backKey = digitalRead(BACK_PIN);

    // 0 means pressed
    // 1 means released
    if (menuKey == 0)
    {
      // open up the menu
      keyboard_key(MENU);
    }

    if (backKey == 0)
    {
      keyboard_key('b');
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
}

#endif