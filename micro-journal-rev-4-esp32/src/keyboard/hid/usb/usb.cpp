#include "usb.h"
//
#include "app/app.h"
#include "config/config.h"
//
#include "keyboard/hid/hid.h"

class MyEspUsbHost : public EspUsbHost
{
  void onReceive(const usb_transfer_t *transfer)
  {
    JsonDocument &app = app_status();
    app["hid_usb"] = true;
  }
  void onGone(const usb_host_client_event_msg_t *eventMsg)
  {
    JsonDocument &app = app_status();
    app["hid_usb"] = false;
  }

  // USB Keycode is sent when key is pressed or released
  void onKeyboard(hid_keyboard_report_t report, hid_keyboard_report_t last_report)
  {
#ifdef DEBUG
    app_log("%02x %02x %02x %02x %02x %02x %02x %02x\n", report.modifier, report.reserved, report.keycode[0], report.keycode[1], report.keycode[2], report.keycode[3], report.keycode[4], report.keycode[5]);
#endif

    // Key Pressed
    for (int i = 0; i < 6; i++)
    {
      if (report.keycode[i] != 0)
      {
        // check if the same key appear in the previous report
        bool newkey = true;
        for (int j = 0; j < 6; j++)
        {
          if (last_report.keycode[j] == report.keycode[i])
          {
            newkey = false;
            break;
          }
        }

        // otherwise register new key press
        if (newkey)
        {
          // handle key pressed
          keyboard_hid_pressed(report.keycode[i], report.modifier);
        }
      }
    }

    // Key Released
    for (int i = 0; i < 6; i++)
    {
      if (last_report.keycode[i] != 0)
      {
        bool key_released = true;
        for (int j = 0; j < 6; j++)
        {
          if (last_report.keycode[i] == report.keycode[j])
          {
            key_released = false;
            break;
          }
        }
        if (key_released)
        {
          keyboard_hid_released(last_report.keycode[i], report.modifier);
        }
      }
    }
  }
};

// Handler for USB Keyboard
MyEspUsbHost usbHost;

//
void keyboard_usb_setup()
{
  //
  app_log("Initializing USB Host\n");

  // usb host setup
#ifndef DEBUG
  usbHost.begin();
#endif
  delay(5000);
}

//
void keyboard_usb_loop()
{
  // Process USB Keyboard Tasks
#ifndef DEBUG
  usbHost.task();
#endif

#ifdef DEBUG

  static int seq_cnt = 0;
  // int sequence[] = {0x29, 0x07}; ESC, D

  // ESC W 1 TEST ENTER PASS ENTER
  //int sequence[] = {0x29, 0x1a, 0x1e, 0x17, 0x08, 0x16, 0x17, 0x28, 0x13, 0x13, 0x28};

  // ESC W 1 TEST ENTER PASS ENTER B S
  int sequence[] = {0x29, 0x1a, 0x1e, 0x17, 0x08, 0x16, 0x17, 0x28, 0x13, 0x13, 0x28};

  // simulate typingis
  static unsigned int last = millis();
  static int index = 0;
  if (millis() - last > 2000)
  {
    last = millis();

    /*    
    keyboard_hid_pressed(sequence[seq_cnt], 0);
    seq_cnt++;
    if (seq_cnt >= sizeof(sequence) / sizeof(int))
    {
      seq_cnt = 0;
      delay(5000);
    }
    */

    // ESC
    // keyboard_hid_pressed(0x29, 0);

    /*
    // simluate backspace
    keyboard_hid_pressed(0x2a, 0);
    delay(100);
    keyboard_hid_released(0x2a, 0);
    */

    /*
    keyboard_hid_pressed(0x1e + index, 0);
    // app_log("Key pressed: %d\n", 0x1e + index);
    index++;
    if (index > 9)
    {
      index = 0;
      // space
      //keyboard_hid_pressed(0x2c, 0);
      // enter
      keyboard_hid_pressed(0x28, 0);
      // delay(5000);
    }
    */
  }

#endif
}
