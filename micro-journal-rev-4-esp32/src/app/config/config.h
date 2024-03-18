#ifndef Config_h
#define Config_h

#include <ArduinoJson.h>

/* You only need to format SPIFFS the first time you run a
   test or else use the SPIFFS plugin to create a partition
   https://github.com/me−no−dev/arduino−esp32fs−plugin */
#define FORMAT_SPIFFS_IF_FAILED true

//
void config_setup();

//
void config_load();
void config_save();

#endif