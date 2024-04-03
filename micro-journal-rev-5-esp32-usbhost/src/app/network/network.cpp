#include "network.h"
#include "../app.h"
#include "../config/config.h"

//
#include "./wifi/wifi.h"

void network_setup()
{
    app_log("\n");
    app_log("Network Setup\n");
    app_log("----------------------\n");

    // read the configuration
    JsonDocument &app = app_status();
    JsonObject config = app["config"];

    // if network is not defined in the config
    // save default configuration
    if (!config.containsKey("network"))
    {
        //
        JsonObject network = config["network"].to<JsonObject>();

        // save config file
        app_log("network configuration is not defined. Creating default config.\n");

        // default value
        network["type"] = "wifi";

        // save the config
        config_save();
    }

    //
    JsonObject network = config["network"];

    // retrieve network type
    String network_type = network["type"].as<String>();
    app_log("network type: %s\n", network_type.c_str());

    if (!network.containsKey("type") || network_type.isEmpty() || network_type == NULL)
    {
        // if the type is empty then default to wifi
        network_type = "wifi";
        network["type"] = "wifi";
        app_log("Network type not speficied. Defaults to wifi\n");

        // save the config
        config_save();
    }

    // setup a default network status
    network["status"] = 0;
}

void network_loop()
{
    // run the loop based on the network type
    static unsigned int last = 0;
    if (millis() - last > 10000)
    {
        //
        last = millis();

        //
        JsonDocument &app = app_status();
        String network_type = app["config"]["network"]["type"].as<String>();

        // by default network is not enabled
        bool network_enabled = app["network"]["enabled"].as<bool>();
        if (network_enabled)
        {
            //
            if (network_type == "wifi")
            {
                network_wifi_loop();
            }
        }
        else
        {
            if (network_type == "wifi")
            {
                // turn off if the network is on
                if (app["network"]["status"].as<int>() > 0)
                {
                    network_wifi_off();
                }
            }
        }
    }
}

void network_enable(bool enable)
{
    //
    JsonDocument &app = app_status();
    app["network"]["enabled"] = enable;
}
