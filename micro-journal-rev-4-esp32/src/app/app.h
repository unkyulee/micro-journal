#ifndef App_h
#define App_h

#include <ArduinoJson.h>

// app version
#define VERSION "1.0.12"

// app status
JsonDocument &app_status();

//
void app_log(const char *format, ...);

#endif