#include "config.h"
#include "../app.h"
#include "app/SD/sd.h"

#include <SD.h>

//
void config_setup()
{
    app_log("\n");
    app_log("Config Setup\n");
    app_log("----------------------\n");

    // load config
    config_load();
}

//
void config_load()
{
    //
    app_log("Loading config ...\n");

    // load app status
    JsonDocument &app = app_status();
    app_log("App Status loaded\n");

    // load config.json
    app_log("Opening config.json file\n");
    File configFile = SD.open("/config.json", "r");    
    if (configFile)
    {
        // read the file
        app_log("Reading config.json file\n");
        String configString = configFile.readString();
        app_log("Closing config.json file\n");
        configFile.close();       

        // check if configString is empty
        if (configString.isEmpty())
        {
            // assign empty object
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
