#pragma once

//
#define WIFI_CONFIG_LIST 0
#define WIFI_CONFIG_EDIT_SSID 1
#define WIFI_CONFIG_EDIT_KEY 2

//
void WifiEntry_setup();
void WifiEntry_keyboard(char key);

// service library to setup wifi and connect/disconnect to wifi
void wifi_config_load();
void wifi_config_save();