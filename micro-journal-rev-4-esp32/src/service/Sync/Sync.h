#pragma once

#include <ArduinoJson.h>

// 0 - starting
// 1 - waiting
// 2 - sending
#define SYNC_ERROR -1
#define SYNC_START 0
#define SYNC_STARTED 1
#define SYNC_PROGRESS 2
#define SYNC_COMPLETED 3

//
void sync_init();
void sync_start_request();


// 
void sync_loop();


// SYNC PROCESS
void sync_start();
bool sync_connect_wifi(JsonDocument &app, const char *ssid, const char *password);
void sync_stop();
void sync_send();
