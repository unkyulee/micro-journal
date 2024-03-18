#include "config.h"
#include "../app.h"

// to save the config.json
#include <SPIFFS.h>

//
void config_setup()
{
    app_log("\n");
    app_log("Config Setup\n");
    app_log("----------------------\n");

    // Initialize SPIFFS
    if (!SPIFFS.begin(true))
    {
        app_log("An Error has occurred while mounting SPIFFS");
        return;
    }

    // load config
    config_load();
}

//
void config_load()
{
    // load app status
    JsonDocument &app = app_status();

    // load config.json
    File configFile = SPIFFS.open("/config.json", "r");
    if (configFile)
    {
        // read the file
        String configString = configFile.readString();
        configFile.close();

        // check if configString is empty
        if (configString.isEmpty())
        {
            // assign empty object
            configString = "{}";
        }

#ifdef DEBUG
        app_log("configString: %s\n", configString.c_str());
#endif

        // Prepare a JsonDocument for the configuration
        // The size should be adjusted according to your configuration's needs
        JsonDocument configDoc;

        // convert to JsonObject
        DeserializationError error = deserializeJson(configDoc, configString);
        if (error)
        {
            //
            app_log("config.json deserializeJson() failed: %s\n", error.c_str());
            return;
        }

        // Assign the loaded configuration to "config" property of app
        app["config"] = configDoc.as<JsonObject>();

        // print out the configuration
        app_log("Config loaded successfully!\n");
    }
    else
    {
        // file doesn't exist
        app_log("config.json file doens't exist\n");
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
    File configFile = SPIFFS.open("/config.json", FILE_WRITE);
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
