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

#include "EspUsbHost.h"

class MyEspUsbHost : public EspUsbHost
{
  //
  uint8_t getKeycodeToAscii(uint8_t keycode, uint8_t shift)
  {
    //
    app_log("%d %d\n", keycode, shift);

    //
    static uint8_t const keyboard_conv_table[128][2] = {HID_KEYCODE_TO_ASCII};
    static uint8_t const keyboard_conv_table_ja[128][2] = {HID_KEYCODE_TO_ASCII_JA};

    //
    if (shift > 1)
    {
      shift = 1;
    }

    if (hidLocal == HID_LOCAL_Japan_Katakana)
    {
      // Japan
      return keyboard_conv_table_ja[keycode][shift];
    }
    else
    {
      // US
      return keyboard_conv_table[keycode][shift];
    }
  }


  //
  void onKeyboardKey(uint8_t ascii, uint8_t keycode, uint8_t modifier)
  {

    if (' ' <= ascii && ascii <= '~')
    {
      keyboard_key(ascii);
    }
    else if (ascii == '\r')
    {
      keyboard_key('\n');
    }
    else if (ascii == '\b')
    {
      keyboard_key('\b');
    }
    else if (ascii == 27)
    {
      keyboard_key(MENU);
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
  // usbHost.setHIDLocal(HID_LOCAL_Japan_Katakana);
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