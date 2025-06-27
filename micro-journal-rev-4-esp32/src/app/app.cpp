//
#include "app.h"
#include "display/display.h"

#ifdef BOARD_PICO
#include "app/FileSystem/FileSystemRP2040.h"
#endif

// When app is not ready. Such as file system is not initialized
// then no more operation should occur.
// this is a flag to check if the app is ready
bool _ready = false;
bool app_ready()
{
    return _ready;
}

// Main object for the setup is to initialize the device
// set up the serial communication
// file system will be initialized here
void app_setup()
{
    // Setup Serial Communication
    _log("Setting Baud Rate: %d\n", 115200);
    Serial.begin(115200);

    // File System Check
    if (filesystem_check() == false)
    {
        // if file system check fails then do not proceed further
        _log("File system check failed. Exiting setup.\n");
        return;
    }

    // Firmware Update Check
    if (firmware_check() == true)
    {
        // new firmare is found do not proceed further
        _log("Firmware update found. Exiting setup.\n");
        return;
    }

    // Load Configuration for the first time
    if (config_load() == false)
    {
        _log("Configuration load failed. Exiting setup.\n");
        return;
    }

    // app ready
    _ready = true;
}

// This is a loop where background tasks will be placed
void app_loop()
{
    // wait until the app is ready
    if (app_ready() == false)
    {
        // wait for a while
        delay(100);
        return;
    }

#ifdef BOARD_ESP32_S3
    // check for background task request
    wifi_service_loop();
#endif
}

// status storage
JsonDocument _status;
JsonDocument &status()
{
    return _status;
}

// file system instance
FileSystem *fileSystem = nullptr;
FileSystem *gfs()
{
    if (fileSystem == nullptr)
    {
// Initialize the file system here
#ifdef BOARD_PICO
        fileSystem = new FileSystemRP2040();
        if (!fileSystem->begin())
        {
            //
            JsonDocument &app = status();
            app["error"] = "RP2040 File System Failed\n";
            app["screen"] = ERRORSCREEN; // ERROR SCREEN IS 0
            _log(app["error"]);
        }        
        else
        {
            _log("RP2040 File System Initialized\n");
        }

#endif

#ifdef BOARD_ESP32_S3
        fileSystem = new SDFS();
#endif
    }

    return fileSystem;
}
