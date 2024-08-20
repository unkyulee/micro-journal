#include "config.h"
#include "app/app.h"
#include "SD/sd.h"
#include "display/display.h"

#include <SD.h>

void _set_default_config()
{
    app_log("Creating default config.json template\n");

    //
    JsonDocument &app = app_status();

    // create a default config
    app["config"]["sync"]["url"] = "";    

    //
    config_save();
}

//
void config_setup()
{
    // load config
    config_load();

    //
    app_log("Total heap: %d\n", ESP.getHeapSize());
    app_log("Free heap: %d\n", ESP.getFreeHeap());
    app_log("Total PSRAM: %d\n", ESP.getPsramSize());
    app_log("Free PSRAM: %d\n", ESP.getFreePsram());
}

//
void config_load()
{
    //
    app_log("Loading config ... %s", VERSION);

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

        // check if configString is empty
        if (configString.isEmpty())
        {
            delay(100);
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
    if (app.containsKey("config"))
    {
        String jsonOutput;
        serializeJsonPretty(app["config"], jsonOutput);
        configFile.println(jsonOutput);

// debug
#ifdef DEBUG
        app_log("%s\n", jsonOutput.c_str());
#endif

        //
        app_log("Config updated successfully.\n");
    }
    else
    {
        app_log("No 'config' property found in app Document.\n");
    }

    // close config.json
    configFile.close();
}
