#ifndef __EspUsbHost_H__
#define __EspUsbHost_H__

#include <Arduino.h>
#include <usb/usb_host.h>
#include <class/hid/hid.h>
#include <rom/usb/usb_common.h>

class EspUsbHost
{
public:
  bool isReady = false;
  uint8_t interval;
  unsigned long lastCheck;

  struct endpoint_data_t
  {
    uint8_t bInterfaceNumber;
    uint8_t bInterfaceClass;
    uint8_t bInterfaceSubClass;
    uint8_t bInterfaceProtocol;
    uint8_t bCountryCode;
  };
  endpoint_data_t endpoint_data_list[17];
  uint8_t _bInterfaceNumber;
  uint8_t _bInterfaceClass;
  uint8_t _bInterfaceSubClass;
  uint8_t _bInterfaceProtocol;
  uint8_t _bCountryCode;
  esp_err_t claim_err;

  usb_host_client_handle_t clientHandle;
  usb_device_handle_t deviceHandle;
  uint32_t eventFlags;
  usb_transfer_t *usbTransfer[16];
  uint8_t usbTransferSize;
  uint8_t usbInterface[16];
  uint8_t usbInterfaceSize;

  hid_local_enum_t hidLocal;

  void begin(void);
  void task(void);

  static void _clientEventCallback(const usb_host_client_event_msg_t *eventMsg, void *arg);
  void _configCallback(const usb_config_desc_t *config_desc);
  void onConfig(const uint8_t bDescriptorType, const uint8_t *p);
  static String getUsbDescString(const usb_str_desc_t *str_desc);
  static void _onReceive(usb_transfer_t *transfer);

  static void _printPcapText(const char *title, uint16_t function, uint8_t direction, uint8_t endpoint, uint8_t type, uint8_t size, uint8_t stage, const uint8_t *data);
  esp_err_t submitControl(const uint8_t bmRequestType, const uint8_t bDescriptorIndex, const uint8_t bDescriptorType, const uint16_t wInterfaceNumber, const uint16_t wDescriptorLength);
  static void _onReceiveControl(usb_transfer_t *transfer);

  virtual void onReceive(const usb_transfer_t *transfer) {};
  virtual void onGone(const usb_host_client_event_msg_t *eventMsg) {};

  virtual uint8_t getKeycodeToAscii(uint8_t keycode, uint8_t shift, uint8_t altgr);
  virtual void onKeyboard(hid_keyboard_report_t report, hid_keyboard_report_t last_report);
  virtual void onKeyboardKey(uint8_t ascii, uint8_t keycode, uint8_t modifier);

  virtual void onMouse(hid_mouse_report_t report, uint8_t last_buttons);
  virtual void onMouseButtons(hid_mouse_report_t report, uint8_t last_buttons);
  virtual void onMouseMove(hid_mouse_report_t report);

  void _onDataGamepad();

  void setHIDLocal(hid_local_enum_t code);

  static uint8_t getItem(uint8_t val)
  {
    return val & 0xfc;
  }
};

#endif