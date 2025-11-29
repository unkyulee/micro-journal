#include "Log.h"
#include "app/app.h"
#include <Arduino.h>

#ifdef BOARD_ESP32_S3
// #define COREID xPortGetCoreID()
#define COREID xPortGetCoreID()
#endif

#ifdef BOARD_PICO
#define COREID get_core_num()
#endif

//
// APP LOG
//
void _log(const char *format, ...)
{
    // Buffer to hold the formatted message
    // Adjust the size according to your needs
    char message[1024];

    // Start variadic argument processing
    va_list args;
    va_start(args, format);

    // Format the string
    vsnprintf(message, sizeof(message), format, args);

    // End variadic argument processing
    va_end(args);

    // Print to Serial
    Serial.printf("[%d][%d] %s", COREID, millis(), message);

#if defined(REV7)
    Serial1.printf("[%d][%d] %s", COREID, millis(), message);
#endif

#if defined(DEBUG)
    // Append to file
    File f = gfs()->open("/debug.log", FILE_APPEND);
    if (f)
    {
        f.printf("[%d][%d] %s", COREID, millis(), message);
        f.close();
    }
#endif
}

//
// DEBUG LOG
//
void _debug(const char *format, ...)
{
#if defined(DEBUG)
    // Buffer to hold the formatted message
    // Adjust the size according to your needs
    char message[1024];

    // Start variadic argument processing
    va_list args;
    va_start(args, format);

    // Format the string
    vsnprintf(message, sizeof(message), format, args);

    // End variadic argument processing
    va_end(args);

    Serial.printf("[%d][%d] %s", COREID, millis(), message);
#endif
}
