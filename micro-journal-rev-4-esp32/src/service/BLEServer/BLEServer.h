#pragma once

//
#define BLE_CONFIG_LIST 0
#define BLE_CONFIG_NO_DEVICES 2

// foreground service
void BLEServer_setup(const char*);
void BLEServer_keyboard(char key);

// background service
void BLEServer_init();
void BLEServer_loop();

// scan devices
void BLEServer_scan();
