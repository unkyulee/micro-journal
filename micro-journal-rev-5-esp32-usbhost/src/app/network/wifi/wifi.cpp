#include "app/app.h"
#include <WiFi.h>

#define AP_SEARCH_DELAY 60000

//
void _scan_wifi_networks(JsonDocument &app);
void _find_wifi_networks(JsonDocument &app);
bool _connect_wifi(JsonDocument &app, const char *ssid, const char *password);

//
void _verify_connection(JsonDocument &app);

void network_wifi_off()
{
    WiFi.disconnect();
    WiFi.mode(WIFI_OFF);

    app_log("WiFi Disconnected. \n");

    JsonDocument &app = app_status();
    app["network"]["status"] = 0;
}

void network_wifi_loop()
{
    // read the configuration
    JsonDocument &app = app_status();
    int network_status = app["network"]["status"].as<int>();

    // network status: 0
    // waiting for the list of access point and available access points
    // when not finding access points that allows the connections then it stay in this state
    // when connection gets established it moves to status: 1
    if (network_status == 0)
    {
        // make sure the ap look up is done at least once per minute not more often
        static unsigned int last = -AP_SEARCH_DELAY;
        if (millis() - last > AP_SEARCH_DELAY)
        {
            //
            last = millis();

            //
            app_log("Looking for access points ...\n");
            _scan_wifi_networks(app);

            //
            _find_wifi_networks(app);
        }
    }

    // network status: 1
    // wifi connection has been established
    // verify if the connection is valid and if disconnected then return to the status: 0
    else if (network_status == 1)
    {
        _verify_connection(app);
    }
}

//
void _scan_wifi_networks(JsonDocument &app)
{
    // Scan for available networks
    WiFi.mode(WIFI_STA);
    int networksFound = WiFi.scanNetworks();
    app_log("Found %d networks:\n");

    // reset the network.access_points array
    JsonArray access_points = app["network"]["access_points"].to<JsonArray>();
    access_points.clear();

    // Print information about each network
    for (int i = 0; i < networksFound; ++i)
    {
        //
        String ssid = WiFi.SSID(i);

        // print out the information
        app_log("%d: %s (%d dBm)\n", i + 1, ssid.c_str(), WiFi.RSSI(i));

        // add to the access_points list
        access_points.add(ssid);
    }
}

//
void _find_wifi_networks(JsonDocument &app)
{
    // Load available WiFi networks from the app["network"]["access_points"] array
    JsonArray availableNetworks = app["network"]["access_points"].as<JsonArray>();

    // Load saved WiFi connection information from the app["config"]["access_points"] array
    JsonArray savedAccessPoints = app["config"]["network"]["access_points"].as<JsonArray>();

    // Iterate through each available network
    for (JsonVariant availableNetwork : availableNetworks)
    {
        // available wifi network
        String availableSsid = availableNetwork.as<String>();

        // see if there are any wifi network allowed
        for (JsonVariant savedAccessPoint : savedAccessPoints)
        {
            const char *savedSsid = savedAccessPoint["ssid"];
            const char *savedPassword = savedAccessPoint["password"];

            // Check if the SSID and password match
            if (strcmp(availableSsid.c_str(), savedSsid) == 0)
            {
                // Try to connect to the matching access point
                if (_connect_wifi(app, savedSsid, savedPassword))
                {
                    Serial.println("Connected to a matching WiFi network!");
                    return; // Exit the function if successfully connected
                }
            }
        }
    }

    Serial.println("No matching WiFi networks found.");
}

bool _connect_wifi(JsonDocument &app, const char *ssid, const char *password)
{
    // Connect to WiFi
    WiFi.begin(ssid, password);
    int attempt = 0;
    while (WiFi.status() != WL_CONNECTED && attempt < 100)
    {
        delay(100);
        Serial.print(".");
        attempt++;
    }

    // Update the network_status: 1
    // meaning it is connected
    if (WiFi.status() == WL_CONNECTED)
    {
        // network status 1
        app["network"]["IP"] = WiFi.localIP().toString();
        app["network"]["ssid"] = ssid;
        app["network"]["status"] = 1;

        // print info
        app_log("\nConnected to WiFi: %s, %s\n", ssid, WiFi.localIP().toString().c_str());

        return true;
    }
    else
    {
        app_log("Failed to connect to WiFi. Please check your credentials.\n");
        // You may add additional handling here if needed
        return false; // Failed to connect
    }
}

void _verify_connection(JsonDocument &app)
{
    // verify if the wifi connection is valid and if disconnected
    // try to put it back to the AP look up mode
    if (WiFi.status() != WL_CONNECTED)
    {
        app["network"]["status"] = 0;
        WiFi.disconnect();
        app_log("WiFi Disconnected. \n");
    }
}