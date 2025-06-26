#include "app.h"

#include "FileSystem/FileSystem.h"
#ifdef BOARD_PICO
#include "FileSystem/RP2040/RP2040.h"
#endif

#ifdef BOARD_ESP32_S3
#include "FileSystem/ESP32/ESP32.h"
#endif

// global status
JsonDocument status;
JsonDocument &app_status()
{
    return status;
}

// file system instance
FileSystem *fileSystem = nullptr;
FileSystem *getFileSystem()
{
    if (fileSystem == nullptr)
    {
// Initialize the file system here
#ifdef BOARD_PICO
        fileSystem = (FileSystem *)new RP2040();
#endif

#ifdef BOARD_ESP32_S3
        fileSystem = (FileSystem *)new ESP32();
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



    /*


    // initialize config
    config_setup();

*/
}

void app_loop()
{
    app_log("Device Started. Baud rate: %d\n", 115200);
    delay(1000);
#ifdef BOARD_ESP32_S3
    // check for background task request
    wifi_service_loop();
#endif
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

/*

void _set_default_config()
{
    app_log("Creating default config.json template\n");

    //
    JsonDocument &app = app_status();

    // create a default config
    app["config"]["sync"]["url"] = "";
    app["config"]["ble"]["remote"] = "";

    //
    config_save();
}

//
void config_setup()
{
    // load config
    config_load();
}

//
void config_load()
{
    //
    app_log("Loading config ... %s\n", VERSION);

#ifdef ENV_USBHOST
    app_log(" REV.5 \n");
#endif

#ifdef ENV_KEYBOARD
    app_log(" REV.6 \n");
#endif

    // load app status
    JsonDocument &app = app_status();
    app_log("App Status loaded\n");

    // load config.json
    app_log("Opening config.json file\n");
    File configFile = SD.open("/config.json", "r");
    if (configFile)
    {
        // read the file
        String configString = configFile.readString();
        configFile.close();
        delay(100);

        // check if configString is empty
        if (configString.isEmpty())
        {
            app_log("Configuration is empty setting default\n");
            _set_default_config();

            // to avoid deserialization failure whem empty
            configString = "{}";
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
            app["screen"] = ERRORSCREEN;

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
        delay(100);
        _set_default_config();

        return;
    }
}

//
void config_save()
{
    // load app status
    JsonDocument &app = app_status();

    // save config
    // Open the file for writing
    File configFile = SD.open("/config.json", FILE_WRITE);
    if (!configFile)
    {
        app_log("Failed to open config.json file for writing.\n");
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
        app_log("No 'config' property found in app Document.\n");
    }

    // close config.json
    configFile.close();
}


*/