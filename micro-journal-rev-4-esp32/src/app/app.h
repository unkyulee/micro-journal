#ifndef App_h
#define App_h

#include <ArduinoJson.h>

// app version
#define VERSION "1.0.11.pascal"

// debug flag
//#define DEBUG

// app life cycle
void app_setup();
void app_loop();

// app status
JsonDocument &app_status();

//
void app_log(const char *format, ...);

#endif