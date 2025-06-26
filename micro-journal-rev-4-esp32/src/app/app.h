#ifndef App_h
#define App_h

// app version
#define VERSION "1.0.19.dev"

#include <ArduinoJson.h>
#include "service/FileSystem/FileSystem.h"

#ifdef BOARD_PICO
#include "service/FileSystem/RP2040/RP2040.h"
#endif

#ifdef BOARD_ESP32_S3
#include "service/FileSystem/SD/SD.h"
#endif

// app status
JsonDocument &app_status();

// file system 
FileSystem *app_fs();

// 
void app_setup();
void app_loop();

// logging
void app_log_empty();
void app_log(const char *format, ...);
void debug_log(const char *format, ...);
String format(const char *format, ...);

// configuration
void app_config_setup();
void app_config_load();
void app_config_save();

// firmware update
bool app_filesystem_check();
bool app_firmware_check();

#endif