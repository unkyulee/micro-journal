#ifndef Keyboard_Nimble_h
#define Keyboard_Nimble_h

#include "NimBLEDevice.h"

bool keyboard_ble_setup();
void keyboard_ble_loop();
bool keyboard_ble_connect();
BLEUUID serviceUUID = BLEUUID("1812");
bool ble_connected = false;

void connect(NimBLEAdvertisedDevice);

bool connectToServer(const char* remote, uint8_t type);

#endif