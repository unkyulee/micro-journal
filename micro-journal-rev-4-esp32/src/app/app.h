#ifndef App_h
#define App_h

#include <ArduinoJson.h>

// app version
#define VERSION "1.0.0"

// debug flag
#define DEBUG

// app life cycle
void app_setup_main();
void app_setup_secondary();

void app_loop_main();
void app_loop_secondary();

// app status
JsonDocument &app_status();

//
void app_log(const char *format, ...);

#endif