#include "app.h"
#include <FS.h>
#include <SD.h>

#ifdef BOARD_PICO
#define FILE_APPEND "a"
#endif

// global status
JsonDocument status;

//
// GLOBAL STATUS
//
// app status
JsonDocument &app_status()
{
    return status;
}

//
// APP LOG
//
void app_log(const char *format, ...)
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
    Serial.printf("[%d] %s", millis(), message);

    // Write Log to SD card
    // Check if SD card is initialized and the file exists
    if (SD.exists("/log.txt"))
    {
        File logFile = SD.open("/log.txt", FILE_APPEND);
        if (logFile)
        {
            // Write the log entry to the file
            logFile.printf("[%d] %s", millis(), message);
            logFile.close();
        }
        else
        {
            // Fallback to Serial if the file cannot be opened
            Serial.println("Error opening log.txt file!");
        }
    }
}

//
// DEBUG LOG
//
void debug_log(const char *format, ...)
{
#ifdef DEBUG
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
    Serial.printf("[%d] %s", millis(), message);
#endif
}

String format(const char *format, ...)
{
    char buffer[256]; // Adjust the size according to your needs
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    return String(buffer);
}