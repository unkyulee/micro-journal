#include "WifiService.h"
#include "app/app.h"
#include "config/config.h"

//
#include "display/display.h"

//
#include <SPI.h>
#include <SPIFFS.h>
#include <SD.h>

//
#include <HTTPClient.h>
#include <SD.h>
#include <base64.h>

void wifi_service_loop()
{
    //
    JsonDocument &app = app_status();
    String task = app["task"].as<String>();

    if(task == "sync_start") {
        sync_start();
    }
}

// For safety concerns saving wifi information sd card can easily expose security information
// Wifi Configuration is saved internal storage of ESP32
void wifi_config_load()
{
    //
    JsonDocument &app = app_status();

    // load config.json
    app_log("Opening wifi.json file from internal storage\n");
    File file = SPIFFS.open("/wifi.json", "r");
    if (file)
    {
        // read the file
        app_log("Reading wifi.json file\n");
        String wifiString = file.readString();
        app_log("Closing wifi.json file\n");
        file.close();

        // check if configString is empty
        if (wifiString.isEmpty())
        {
            // to avoid deserialization failure whem empty
            wifiString = "{}";
        }

        // Prepare a JsonDocument for the configuration
        // The size should be adjusted according to your configuration's needs
        JsonDocument configDoc;

        // convert to JsonObject
        DeserializationError error = deserializeJson(configDoc, wifiString);
        app_log("Deserializing wifi.json file\n");
        if (error)
        {
            //
            app_log("wifi.json deserializeJson() failed: %s\n", error.c_str());

            //
            app["error"] = "Wrong format wifi.json";
            app["screen"] = ERRORSCREEN;

            // delete wifi.json from SPIFF
            SPIFFS.remove("/wifi.json");

            return;
        }

        // Assign the loaded configuration to "config" property of app
        app_log("Loading wifi config\n");
        app["wifi"] = configDoc.as<JsonObject>();

        // print out the configuration
        app_log("Wifi config loaded successfully!\n");
    }
    else
    {
        // file doesn't exist
        app_log("wifi.json file doens't exist\n");
        delay(100);

        return;
    }
}

// save current wifi configuration
void wifi_config_save()
{
    // load app status
    JsonDocument &app = app_status();

    // save config
    // Open the file for writing
    File file = SPIFFS.open("/wifi.json", FILE_WRITE);
    if (!file)
    {
        app_log("Failed to open wifi.json file for writing.\n");
        return;
    }

    // Serialize the "config" property of the app Document directly to the file
    if (app["wifi"].is<JsonObject>())
    {
        String jsonOutput;
        serializeJsonPretty(app["wifi"], jsonOutput);
        file.println(jsonOutput);

// debug
#ifdef DEBUG
        app_log("%s\n", jsonOutput.c_str());
#endif

        //
        app_log("Wifi config updated successfully.\n");
    }
    else
    {
        app_log("No 'wifi' property found in app Document.\n");
    }

    // close config.json
    file.close();
}

// Reset all the sync related flags
void wifi_sync_init()
{
    // reset the sync state
    // update app sync state
    JsonDocument &app = app_status();
    app["sync_state"] = 0;
    app["sync_error"] = "";
}

// start WIFI
void sync_start_request()
{
    JsonDocument &app = app_status();
    app["task"] = "sync_start";
}

// Start Sync Process
// Search for WIFI ACCESS POINTS
void sync_start()
{
    //
    JsonDocument &app = app_status();

    ptft->setTextColor(TFT_WHITE, TFT_BLACK);
    ptft->println(" - Scanning WIFI ...");

    // Scan for available networks
    WiFi.mode(WIFI_STA);
    WiFi.setHostname("MICROJOURNAL");

    // wait for decent amount of time before using wifi
    delay(3000);

    //
    int networksFound = WiFi.scanNetworks();
    app_log("Found %d networks:\n", networksFound);

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
    //
    ptft->printf(" - Trying to connect to WIFI\n", networksFound);

    //
    // Load available WiFi networks from the app["network"]["access_points"] array
    JsonArray availableNetworks = app["network"]["access_points"].as<JsonArray>();

    // Load saved WiFi connection information from the app["config"]["access_points"] array
    JsonArray savedAccessPoints = app["wifi"]["access_points"].as<JsonArray>();

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

                    //
                    ptft->printf(" - Connected to: %s\n", savedSsid);
                    delay(1000);

                    //
                    sync_state = SYNC_SEND;

                    return; // Exit the function if successfully connected
                }
            }
        }
    }

    //
    _network_stop();

    //
    sync_error = "NOT ABLE TO CONNECT TO WIFI";
    sync_state = -1;
    //
}

bool _connect_wifi(JsonDocument &app, const char *ssid, const char *password)
{
    // Connect to WiFi
    delay(1000);
    app_log("trying to connect to %s\n", ssid);
    WiFi.begin(ssid, password);
    delay(1000);

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
        //
        sync_error = "NOT ABLE TO CONNECT TO WIFI ";
        sync_error = sync_error + ssid;
        sync_state = -1;

        return false; // Failed to connect
    }
}

void _network_stop()
{
    WiFi.disconnect();
    WiFi.mode(WIFI_OFF);
}

#define BASE64FILE "/base64.txt"
void _sync_send(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    //
    JsonDocument &app = app_status();

    ptft->setTextColor(TFT_WHITE, TFT_BLACK);

    // preparing the file
    ptft->println(" - Preparing the file ... ");
    {
        File inputFile = SD.open(Editor::getInstance().fileBuffer.getFileName());
        if (!inputFile)
        {
            Serial.println("Error opening input file!");
            return;
        }

        File outputFile = SD.open(BASE64FILE, FILE_WRITE);
        if (!outputFile)
        {
            Serial.println("Error opening output file!");
            inputFile.close();
            return;
        }

        //
        const int bufferSize = 3000; // base64 encoding is done per 3 bytes
        uint8_t buffer[bufferSize];
        while (inputFile.available())
        {
            size_t bytesRead = inputFile.read(buffer, bufferSize);
            if (bytesRead > 0)
            {
                // Encode buffer to base64 and write to output file
                String txtBase64 = base64::encode(buffer, bytesRead);
                outputFile.print(txtBase64);
            }
        }

        inputFile.close();
        outputFile.close();
    }

    //
    ptft->println(" - Sending file to drive ... ");

    // send file to google drive
    String url = app["config"]["sync"]["url"].as<String>();
    if (!url.isEmpty() && url != "null")
    {
        app_log("Requesting sync ... \n");
        app_log("%s\n", url.c_str());

        // prepare http client
        HTTPClient http;
        http.begin(url);
        http.setFollowRedirects(HTTPC_FORCE_FOLLOW_REDIRECTS);

        // Read file in chunks and send via POST request
        File file = SD.open(BASE64FILE, FILE_READ);
        if (!file)
        {
            ptft->println("Failed to open file");
            http.end();
            return;
        }

        //
        http.sendRequest("POST", &file, file.size());

        // close http connection
        http.end();

        // Close file
        file.close();

        //
        sync_state = SYNC_COMPLETED;
        Menu_clear();
    }
    else
    {
        // close WIFI
        _network_stop();

        //
        sync_error = "SYNC URL NOT FOUND";
        sync_state = SYNC_ERROR;
        Menu_clear();
    }
}

int sync_state = 0;
int sync_state_prev = -1;
String sync_error = "";

void _network_stop()
{
    WiFi.disconnect();
    WiFi.mode(WIFI_OFF);
}

#define BASE64FILE "/base64.txt"
void _sync_send(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    //
    JsonDocument &app = app_status();

    ptft->setTextColor(TFT_WHITE, TFT_BLACK);

    // preparing the file
    ptft->println(" - Preparing the file ... ");
    {
        File inputFile = SD.open(Editor::getInstance().fileBuffer.getFileName());
        if (!inputFile)
        {
            Serial.println("Error opening input file!");
            return;
        }

        File outputFile = SD.open(BASE64FILE, FILE_WRITE);
        if (!outputFile)
        {
            Serial.println("Error opening output file!");
            inputFile.close();
            return;
        }

        //
        const int bufferSize = 3000; // base64 encoding is done per 3 bytes
        uint8_t buffer[bufferSize];
        while (inputFile.available())
        {
            size_t bytesRead = inputFile.read(buffer, bufferSize);
            if (bytesRead > 0)
            {
                // Encode buffer to base64 and write to output file
                String txtBase64 = base64::encode(buffer, bytesRead);
                outputFile.print(txtBase64);
            }
        }

        inputFile.close();
        outputFile.close();
    }

    //
    ptft->println(" - Sending file to drive ... ");

    // send file to google drive
    String url = app["config"]["sync"]["url"].as<String>();
    if (!url.isEmpty() && url != "null")
    {
        app_log("Requesting sync ... \n");
        app_log("%s\n", url.c_str());

        // prepare http client
        HTTPClient http;
        http.begin(url);
        http.setFollowRedirects(HTTPC_FORCE_FOLLOW_REDIRECTS);

        // Read file in chunks and send via POST request
        File file = SD.open(BASE64FILE, FILE_READ);
        if (!file)
        {
            ptft->println("Failed to open file");
            http.end();
            return;
        }

        //
        http.sendRequest("POST", &file, file.size());

        // close http connection
        http.end();

        // Close file
        file.close();

        //
        sync_state = SYNC_COMPLETED;
        Menu_clear();
    }
    else
    {
        // close WIFI
        _network_stop();

        //
        sync_error = "SYNC URL NOT FOUND";
        sync_state = SYNC_ERROR;
        Menu_clear();
    }
}
