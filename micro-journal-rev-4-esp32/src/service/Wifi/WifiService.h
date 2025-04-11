#ifndef WifiService_h
#define WifiService_h

#include <ArduinoJson.h>

// 
void wifi_service_loop();

// service library to setup wifi and connect/disconnect to wifi
void wifi_config_load();
void wifi_config_save();

// 0 - starting
// 1 - waiting
// 2 - sending
#define SYNC_START 0
#define SYNC_PROGRESS 1
#define SYNC_ERROR -1
#define SYNC_COMPLETED 2

void wifi_sync_init();

// SYNC PROCESS
void sync_start_request();
void sync_start();
bool sync_connect_wifi(JsonDocument &app, const char *ssid, const char *password);
void sync_stop();
void sync_send();

#endif