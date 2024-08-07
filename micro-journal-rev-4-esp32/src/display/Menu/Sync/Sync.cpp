#include "Sync.h"
#include "../Menu.h"
#include "app/app.h"
#include "config/config.h"
#include "display/display.h"
#include "editor/editor.h"
#include "../Wifi/Wifi.h"

//
#include <HTTPClient.h>
#include <SD.h>
#include <base64.h>

// 0 - starting
// 1 - waiting
// 2 - sending
#define SYNC_START 0
#define SYNC_ERROR -1
#define SYNC_SEND 1
#define SYNC_COMPLETED 2
int sync_state = 0;
int sync_state_prev = -1;
String sync_error = "";

//
void _sync_start(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f);
bool _connect_wifi(JsonDocument &app, const char *ssid, const char *password);
void _sync_send(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f);
void _network_stop();

//
void Sync_setup(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    // when entering the screen
    // clear the screen
    Menu_clear();

    // load wifi
    Wifi_load();

    // reset sync state
    sync_state = 0;

    //
    sync_error = "";
}

//
void Sync_render(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    if (sync_state_prev != sync_state)
        Menu_clear();

    //
    JsonDocument &app = app_status();

    // header
    ptft->setCursor(0, 30, 2);
    ptft->setTextColor(TFT_WHITE, TFT_BLACK);
    ptft->println(" SYNC IN PROGRESS ");

    //
    if (sync_state == SYNC_ERROR)
    {
        // error
        //
        ptft->setTextColor(TFT_WHITE, TFT_RED);
        ptft->println(sync_error);
        ptft->setTextColor(TFT_WHITE, TFT_BLACK);
    }
    else if (sync_state == SYNC_START)
    {
        // start network
        sync_error = "";
        _sync_start(ptft, pu8f);
    }
    else if (sync_state == SYNC_SEND)
    {
        _sync_send(ptft, pu8f);
    }
    else if (sync_state == SYNC_COMPLETED)
    {
        // close WIFI
        _network_stop();

        //
        ptft->println(" - SYNC Completed");
    }

    ptft->println("");
    ptft->println(" press [B] to exit ");

    //
    sync_state_prev = sync_state;
}

//
void Sync_keyboard(char key)
{
    //
    JsonDocument &app = app_status();

    // when pressed B
    if (key == '\b' || key == 'b' || key == 'B')
    {
        // stop network
        _network_stop();

        //
        sync_state = -1;
    }

    //
    app["menu"]["state"] = MENU_HOME;
}

// start WIFI
void _sync_start(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
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
