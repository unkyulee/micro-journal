#ifndef App_h
#define App_h

#include <ArduinoJson.h>

// app version
#define VERSION "1.0.18.r2"

// app status
JsonDocument &app_status();

//
void app_log(const char *format, ...);
void debug_log(const char *format, ...);
String format(const char *format, ...);

#endif