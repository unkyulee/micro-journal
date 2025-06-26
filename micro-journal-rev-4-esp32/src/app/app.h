#ifndef App_h
#define App_h

#include <ArduinoJson.h>

// app version
#define VERSION "1.0.19.dev"

// app status
JsonDocument &app_status();

// 
void app_setup();
void app_loop();

//
void app_log(const char *format, ...);
void debug_log(const char *format, ...);
String format(const char *format, ...);


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