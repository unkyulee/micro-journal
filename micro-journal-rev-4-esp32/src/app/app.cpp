//
#include "app.h"
#include "display/display.h"

//
#include "service/WordCounter/WordCounter.h"
#include "service/Send/Send.h"

#ifdef SD_CS
#include "app/FileSystem/FileSystemSD.h"
#endif

#ifdef USE_FAT
#include "app/FileSystem/FileSystemFAT.h"
#endif

#ifdef BOARD_PICO
#include "app/FileSystem/FileSystemRP2040.h"
#endif

#ifdef USE_MSC
#include "service/MassStorage/MassStorage.h"
#endif

#ifdef BOARD_ESP32_S3
#include "service/Sync/Sync.h"
#endif

#ifdef BATTERY
#include "service/Battery/Battery.h"
#endif

#ifdef USE_BLESERVER
#include "service/BLEServer/BLEServer.h"
#endif

#ifdef CARDPUTER
#include "M5Cardputer.h"
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
#ifdef DEBUG
    delay(5000);
#endif

#ifdef CARDPUTER
    auto cfg = M5.config();
    M5Cardputer.begin(cfg, true); // enableKeyboard
#endif

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

#ifdef USE_MSC
    // Mass Storage Setup
    ms_setup();
#endif

#ifdef BATTERY
    battery_setup();
#endif

#if defined(REV5) || defined(CARDPUTER)
    // BLE Server Background Task
    BLEServer_init();
#endif

#ifdef BOARD_ESP32_S3
    // lower the CPU clock to extend battery life
    // sync and mass storage restore full speed while they run
    setCpuFrequencyMhz(CPU_FREQUENCY_LOW);
    _log("CPU frequency set to %d MHz\n", getCpuFrequencyMhz());
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

    // word count
    wordcounter_service();

    // SEND feature
    send_loop();

#ifdef USE_BLESERVER
    // Pairing with BLE Keyboard Enabled
    BLEServer_loop();
#endif

#ifdef BATTERY
    battery_loop();
#endif

#ifdef USE_MSC
    // Mass Storage Control Loop
    ms_loop();
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

// ESP32-S3 boards with SD configured use SD; otherwise use internal SPIFFS.
#ifdef SD_CS

        // Use SD Card
        fileSystem = new FileSystemSD();

#endif

#ifdef USE_MSC

// Initialize the file system here
#ifdef BOARD_PICO
        fileSystem = new FileSystemRP2040();
#endif

// Initialize the file system here
#ifdef BOARD_ESP32_S3
        // file system in FAT
        fileSystem = new FileSystemFAT("storage");
#endif

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
