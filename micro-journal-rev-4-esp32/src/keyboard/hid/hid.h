#ifndef HID_h
#define HID_h

//
#include <EspUsbHost.h>

// Handle Keyboard events coming from USB and Bluetooth (BLE) Keyboards

void keyboard_hid_setup();
void keyboard_hid_loop();

// HID should callback key pressed and released 
void keyboard_hid_pressed(uint8_t keycode, uint8_t modifier);
void keyboard_hid_released(uint8_t keycode, uint8_t modifier);

#endif