#include "app.h"
#include "display/display.h"
#include <SPI.h>

#ifdef BOARD_ESP32_S3
#include <SPIFFS.h>
#include <SD.h>
#endif


// global status
JsonDocument status;
JsonDocument &app_status()
{
    return status;
}

// file system instance
FileSystem *fileSystem = nullptr;
FileSystem *app_fs()
{
    if (fileSystem == nullptr)
    {
// Initialize the file system here
#ifdef BOARD_PICO
        fileSystem = (FileSystem *)new RP2040();
#endif

#ifdef BOARD_ESP32_S3
        fileSystem = (FileSystem *)new SD();
#endif
    }

    return fileSystem;
}

// Main object for the setup is to initialize the device
// set up the serial communication
// file system will be initialized here
void app_setup()
{
    // Setup Serial Communication
    app_log("Device Started. Baud rate: %d\n", 115200);
    Serial.begin(115200);

    // File System Check
    if(app_filesystem_check() == false) {
        // if file system check fails then do not proceed further
        return;
    }

    // Firmware Update Check
    if(app_firmware_check() == true) {
        // new firmare is found do not proceed further
        return;
    }

    // Empty log.txt file
    app_log_empty();

    // Load Configuration for the first time
    app_config_load();
}

void app_loop()
{
#ifdef BOARD_ESP32_S3
    // check for background task request
    wifi_service_loop();
#endif
}

// check if file system is correctly loaded
bool app_filesystem_check()
{    
    //
    JsonDocument &app = app_status();

#ifdef BOARD_ESP32_S3
    // CHECK IF SPIFFS is formatted
    // Mount SPIFFS
    if (!SPIFFS.begin())
    {
        //
        app_log("SPIFFS mount failed!\n");

        //
        app["error"] = "SPIFFS NOT FORMATTED";
        app["screen"] = ERRORSCREEN

        return false;
    }

    // Initialize SD Card
#if defined(ENV_EPAPER)
    // 
    SPI.begin(SD_SCLK, SD_MISO, SD_MOSI, SD_CS);
    if (!SD.begin(SD_CS, SPI))
#else
    //
    if (!SD.begin(SD_CS))
#endif
    {
        //
        app_log("Card Mount Failed\n");

        //
        app["error"] = " SD CARD MISSING ";
        app["screen"] = ERRORSCREEN;

        return false;
    }

    // Check for card type
    app_log("SD Device Initialized\n");
    uint8_t cardType = SD.cardType();

    if (cardType == CARD_NONE)
    {
        //
        app_log("No SD card attached\n");
        //
        app["error"] = " SD CARD NOT DETECTED ";
        app["screen"] = ERRORSCREEN;

        return false;
    }

    //
    app_log("SD Card detected\n");
#endif

    // file system check pass
    return true;
}

// Clear log.txt content when boot
void app_log_empty()
{
    File logFile = app_fs()->open("/log.txt", "w");
    logFile.close();
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

    // Write Log to the main file system
    File logFile = app_fs()->open("/log.txt", "a");
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

// Default Configuration
void _set_default_config()
{
    app_log("Creating default config.json template\n");

    //
    JsonDocument &app = app_status();

    // create a default config
    app["config"]["sync"]["url"] = "";
    app["config"]["ble"]["remote"] = "";

    //
    app_config_save();
}

//
void app_config_load()
{
    //
    app_log("Loading config ... %s\n", VERSION);

    // load app status
    JsonDocument &app = app_status();

    // load config.json
    app_log("Opening config.json file\n");
    File configFile = app_fs()->open("/config.json", "r");
    if (configFile)
    {
        // read the file
        String configString = configFile.readString();
        configFile.close();
        delay(100); // safe to wait for a few seconds after closing a file

        // check if configString is empty
        if (configString.length() == 0)
        {
            // create a default config.json
            _set_default_config();

            // configuration is loaded with default
            // no need to proceed further
            return;
        }

        // Prepare a JsonDocument for the configuration
        // The size should be adjusted according to your configuration's needs
        JsonDocument configDoc;

        // convert to JsonObject
        DeserializationError error = deserializeJson(configDoc, configString);
        app_log("Deserializing config.json file\n");
        if (error)
        {
            //
            app_log("config.json deserializeJson() failed: %s\n", error.c_str());

            //
            app["error"] = "Wrong format config.json";
            app["screen"] = ERRORSCREEN; // ERROR SCREEN IS 0

            return;
        }

        // Assign the loaded configuration to "config" property of app
        app_log("Loading app status config\n");
        app["config"] = configDoc.as<JsonObject>();

        // print out the configuration
        app_log("Config loaded successfully!\n");
    }
    else
    {
        // file doesn't exist
        app_log("config.json file doens't exist\n");

        // create a default config.json
        _set_default_config();

        return;
    }
}

//
void app_config_save()
{
    // load app status
    JsonDocument &app = app_status();

    // save config
    // Open the file for writing
    File configFile = app_fs()->open("/config.json", "w");
    if (!configFile)
    {
        //
        app["error"] = "Failed to open config.json file for writing.\n";
        app["screen"] = ERRORSCREEN; // ERROR SCREEN IS 0

        app_log(app["error"]);

        return;
    }

    // Serialize the "config" property of the app Document directly to the file
    if (app["config"].is<JsonObject>())
    {
        String jsonOutput;
        serializeJsonPretty(app["config"], jsonOutput);
        configFile.println(jsonOutput);

        // debug output configuration content
        debug_log("config_save\n%s\n", jsonOutput.c_str());

        //
        app_log("Config updated successfully.\n");
    }
    else
    {
        //
        app["error"] = "No 'config' property found in app Document.\n";
        app["screen"] = 0; // ERROR SCREEN IS 0

        app_log(app["error"]);
    }

    // close config.json
    configFile.close();
}

// check firmware update
bool app_firmware_check()
{    
    // load app status
    JsonDocument &app = app_status();
    
    // Check if there are firmware.bin in the SD card
    if (app_fs()->exists(FIRMWARE))
    {
        // move to firmware update screen
        app["screen"] = MENUSCREEN;
        app["menu"]["state"] = MENU_FIRMWARE;

        return false;
    }

    return true;
}