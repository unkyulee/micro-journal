#ifndef Keyboard_Nimble_h
#define Keyboard_Nimble_h

#include "NimBLEDevice.h"

// Initialize BLE DEVICE
void ble_init(const char* name);

// Connect to BLE Keyboard
bool ble_connect(const char *address);

//
void ble_setup(const char* adName);
void ble_loop();

#endif