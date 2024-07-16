#ifndef App_h
#define App_h

#include <ArduinoJson.h>

// app version
#define VERSION "1.0.14.DEV"

// app status
JsonDocument &app_status();

//
void app_log(const char *format, ...);
String format(const char *format, ...);

#endif