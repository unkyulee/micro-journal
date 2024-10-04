#ifndef Keyboard_Nimble_h
#define Keyboard_Nimble_h

#include "NimBLEDevice.h"

bool keyboard_ble_setup();
void keyboard_ble_loop();
bool keyboard_ble_connect();

//
void connect(NimBLEAdvertisedDevice);
bool connectToServer(const char* remote, uint8_t type);

//
BLEUUID keyboard_ble_service_uuid();

//
bool keyboard_ble_connected();
void keyboard_ble_connected_set(bool connected);

#endif