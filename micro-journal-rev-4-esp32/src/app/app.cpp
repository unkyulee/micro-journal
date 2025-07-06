//
#include "app.h"
#include "display/display.h"

#ifdef BOARD_ESP32_S3
#include "app/FileSystem/FileSystemSD.h"
#include "app/FileSystem/FileSystemSPIFFS.h"
#include "service/Sync/Sync.h"
#endif

#ifdef BOARD_PICO
#include "app/FileSystem/FileSystemRP2040.h"
#include "service/MassStorage/MassStorage.h"
#endif

#ifdef BATTERY
#include "service/Battery/Battery.h"
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
    Serial.begin(115200);
    _log("Setting Baud Rate: %d\n", 115200);

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

#ifdef BATTERY
    battery_setup();
#endif

    // app ready
    _ready = true;
    _log("App is ready\n");
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

#ifdef BOARD_PICO
    // Mass Storage Control Loop
    ms_loop();
#endif

#ifdef BATTERY
    battery_loop();
#endif

#ifdef BOARD_ESP32_S3
    sync_loop();
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
#endif

#ifdef BOARD_ESP32_S3
        // ESP32 will use SD card as a main file system
        fileSystem = new FileSystemSD();
#endif
        if (!fileSystem->begin())
        {
            //
            JsonDocument &app = status();
            app["error"] = "File System Failed\n";
            app["screen"] = ERRORSCREEN; // ERROR SCREEN IS 0
            _log(app["error"]);
        }
        else
        {
            _log("File System Initialized\n");
        }
    }

    return fileSystem;
}

#ifdef BOARD_ESP32_S3
FileSystem *_spiffs = nullptr;
// ESP32 has SPIFFS as internal file system
FileSystem *spiffs()
{
    if (_spiffs == nullptr)
    {
        _spiffs = new FileSystemSPIFFS();
        if (!_spiffs->begin())
        {
            //
            JsonDocument &app = status();
            app["error"] = "Internal File System Failed\n";
            app["screen"] = ERRORSCREEN; // ERROR SCREEN IS 0
            _log(app["error"]);
        }
        else
        {
            _log("Internal File System Initialized\n");
        }
    }

    return _spiffs;
}
#endif
