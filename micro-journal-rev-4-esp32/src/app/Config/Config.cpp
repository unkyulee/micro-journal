#include "Config.h"
#include "app/app.h"
#include "display/display.h"


// Default Configuration
void _set_default_config()
{
    // load app status
    JsonDocument &app = status();
    _log("Creating default config.json template\n");
    
    // create a default config
    app["config"]["sync"]["url"] = "";
    app["config"]["ble"]["remote"] = "";

    //
    config_save();
}

//
bool config_load()
{
    //
    _log("Loading config ... %s\n", VERSION);

    // load app status
    JsonDocument &app = status();

    // load config.json
    _log("Opening config.json file\n");
    File configFile = gfs()->open("/config.json", "r");
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
            return true;
        }

        // Prepare a JsonDocument for the configuration
        // The size should be adjusted according to your configuration's needs
        JsonDocument configDoc;

        // convert to JsonObject
        DeserializationError error = deserializeJson(configDoc, configString);
        _log("Deserializing config.json file\n");
        if (error)
        {
            //
            _log("config.json deserializeJson() failed: %s\n", error.c_str());

            //
            app["error"] = "Wrong format config.json";
            app["screen"] = ERRORSCREEN; // ERROR SCREEN IS 0

            return false;
        }

        // Assign the loaded configuration to "config" property of app
        _log("Loading app status config\n");
        app["config"] = configDoc.as<JsonObject>();

        // print out the configuration
        _log("Config loaded successfully!\n");
    }
    else
    {
        // file doesn't exist
        _log("config.json file doens't exist\n");

        // create a default config.json
        _set_default_config();        
    }

    return true;
}

//
void config_save()
{
    // load app status
    JsonDocument &app = status();

    // save config
    // Open the file for writing
    File configFile = gfs()->open("/config.json", "w");    
    if (!configFile)
    {
        //
        app["error"] = "Failed to open config.json file for writing.\n";
        app["screen"] = ERRORSCREEN; // ERROR SCREEN IS 0

        _log(app["error"]);

        return;
    }

    // Serialize the "config" property of the app Document directly to the file
    if (app["config"].is<JsonObject>())
    {
        String jsonOutput;
        serializeJsonPretty(app["config"], jsonOutput);
        configFile.println(jsonOutput);

        // debug output configuration content
        //_debug("config_save\n%s\n", jsonOutput.c_str());

        //
        //_log("Config updated successfully.\n");
    }
    else
    {
        //
        app["error"] = "No 'config' property found in app Document.\n";
        app["screen"] = 0; // ERROR SCREEN IS 0

        _log(app["error"]);
    }

    // close config.json
    configFile.close();
    delay(100);
}