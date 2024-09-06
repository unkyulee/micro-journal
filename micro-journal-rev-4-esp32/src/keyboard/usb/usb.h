#ifndef Keyboard_USB_h
#define Keyboard_USB_h

#include <Arduino.h>

void keyboard_usb_setup();
void keyboard_usb_loop();
void keyboard_layout(String layout);
uint8_t getKeycodeToAscii(uint8_t keycode, uint8_t shift, uint8_t altgr);
void onKeyboardKey(uint8_t ascii, uint8_t keycode, uint8_t modifier);

#endif
