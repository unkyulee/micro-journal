//
#include "app.h"
#include "display/display.h"

//
#include "service/WordCounter/WordCounter.h"

#ifdef BOARD_ESP32_S3
#include "app/FileSystem/FileSystemSD.h"
#include "service/Sync/Sync.h"
#include "service/Send/Send.h"
#endif

#ifdef BOARD_PICO
#include "app/FileSystem/FileSystemRP2040.h"
#include "service/MassStorage/MassStorage.h"
#include "service/Send/Send.h"
#endif

#ifdef BATTERY
#include "service/Battery/Battery.h"
#endif

#ifdef REV5
#include "service/BLEServer/BLEServer.h"
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
#if defined(REV7)
    const int RX_PIN = 45;
    const int TX_PIN = 48;
    Serial1.begin(115200, SERIAL_8N1, RX_PIN, TX_PIN);
#endif
    _log("Setting Baud Rate: %d\n", 115200);

#if defined(BOARD_HAS_PSRAM)
    // Check if PSRAM is detected and enabled
    if (psramFound())
    {
        Serial.println("PSRAM is detected and enabled!");
        Serial.printf("Total heap: %d bytes\n", ESP.getHeapSize());
        Serial.printf("Total PSRAM: %d bytes\n", ESP.getPsramSize());
    }
    else
    {
        Serial.println("PSRAM not found or not initialized!");
    }

    // allocate memory is PSRAM
    heap_caps_malloc_extmem_enable(64);

    //
    Serial.println("Flash Chip Information:");
    Serial.printf("  Size: %u bytes (%.2f MB)\n", ESP.getFlashChipSize(), ESP.getFlashChipSize() / 1024.0 / 1024.0);
    Serial.printf("  Speed: %u Hz\n", ESP.getFlashChipSpeed());
    Serial.printf("  Mode: %u\n", ESP.getFlashChipMode());
#endif

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

#ifdef BOARD_PICO
    // Mass Storage Setup
    ms_setup();
#endif

#ifdef BATTERY
    battery_setup();
#endif

#ifdef REV5
    // BLE Server Background Task
    BLEServer_init();
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

    wordcounter_service();

#ifdef BOARD_PICO
    // Mass Storage Control Loop
    ms_loop();

    // SEND background task loop
    send_loop();
#endif

#ifdef BATTERY
    battery_loop();
#endif

#ifdef BOARD_ESP32_S3
    sync_loop();

    // SEND background task loop
    send_loop();
#endif

#ifdef REV5
    // BLE Server Background Task
    BLEServer_loop();
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
