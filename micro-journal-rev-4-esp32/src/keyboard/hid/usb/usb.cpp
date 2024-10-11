#include "usb.h"
//
#include "app/app.h"
#include "config/config.h"
//
#include "keyboard/hid/hid.h"

class MyEspUsbHost : public EspUsbHost
{
  void onReceive(const usb_transfer_t *transfer) {
    JsonDocument &app = app_status();
    app["hid_usb"] = true;
  }
  void onGone(const usb_host_client_event_msg_t *eventMsg) {
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
  app_log("Initializing USB Keyboard\n");

  // usb host setup
  usbHost.begin();
}

//
void keyboard_usb_loop()
{
  // Process USB Keyboard Tasks
  usbHost.task();
}
