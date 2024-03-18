#include "app.h"
#include "config/config.h"
#include "network/network.h"

// global status
JsonDocument status;

//
// SETUP
//
// initialize main task setup
void app_setup_main()
{    
}

// initialize sedcondary task
void app_setup_secondary() 
{
    // initialize config
    config_setup();

    // network setup
    network_setup();
}

//
// LOOP
//
void app_loop_main()
{
}

void app_loop_secondary()
{
    // network
    // network setup runs in the secondary loop
    network_loop();
}

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
    Serial.print(message);
}