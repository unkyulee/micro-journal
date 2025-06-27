#pragma once

// app version
#define VERSION "1.0.19.dev"

// default utility headers
#include <ArduinoJson.h>
#include "Log/Log.h"
#include "filesystem/FileSystem.h"
#include "Config/Config.h"
#include "Verification/Verification.h"
#include "service/Tools/Tools.h"

// 
void app_setup();
void app_loop();

// is app ready?
bool app_ready();

// app status
JsonDocument &status();

// ESP32 has SD or SPIFFS 
// RP2040 has LittleFS
// This is a pattern to hide the implementation of the file system
// and provide a common interface to access the file system
FileSystem *gfs();
