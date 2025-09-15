#include "Sync.h"
#include "app/app.h"
#include "display/display.h"
#include "service/Editor/Editor.h"

//
#include <HTTPClient.h>
#include <SD.h>
#include <base64.h>

// Reset all the sync related flags
void sync_init()
{
    // reset the sync state
    // update app sync state
    JsonDocument &app = status();
    app["sync_state"] = SYNC_START;
    app["sync_error"] = "";
    app["sync_message"] = "";

    _log("[sync_init] sync_init\n");
}

// request background service to pick up the request
void sync_start_request()
{
    JsonDocument &app = status();
    String task = app["task"].as<String>();
    //
    if (task != "sync_start")
    {
        app["task"] = "sync_start";
        _log("[sync_start_request] Sync Start Requested\n");
    }
}

//
void sync_loop()
{
    static unsigned int last = 0;
    if (millis() - last > 1000)
    {
        last = millis();

        //
        JsonDocument &app = status();
        String task = app["task"].as<String>();

        if (task == "sync_start")
        {
            //
            app["task"] = "";

            //
            _log("[sync_loop] Picked up Sync Start Request\n");

            //
            sync_start();
        }
    }
}

// Start Sync Process
// Search for WIFI ACCESS POINTS
void sync_start()
{
    //
    JsonDocument &app = status();

    //
    _log("[sync_start] Sync Start\n");

    //
    app["sync_state"] = SYNC_STARTED;
    app["sync_message"] = "Connecting to WiFi";
    app["clear"] = true;

    // Scan for available networks
    WiFi.mode(WIFI_STA);
    WiFi.setHostname("MICROJOURNAL");

    // wait for decent amount of time before using wifi
    delay(3000);

    //
    int networksFound = WiFi.scanNetworks();
    app["sync_message"] = format("Found %d networks", networksFound);
    app["clear"] = true;

    // reset the network.access_points array
    JsonArray access_points = app["network"]["access_points"].to<JsonArray>();
    access_points.clear();

    // Print information about each network
    for (int i = 0; i < networksFound; ++i)
    {
        //
        String ssid = WiFi.SSID(i);

        // print out the information
        _log("%d: %s (%d dBm)\n", i + 1, ssid.c_str(), WiFi.RSSI(i));

        // add to the access_points list
        access_points.add(ssid);
    }

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
                if (sync_connect_wifi(app, savedSsid, savedPassword))
                {
                    Serial.println("Connected to a matching WiFi network!");

                    //
                    app["sync_message"] = format("Connected to: %s\n", savedSsid);
                    app["clear"] = true;
                    _log(app["sync_message"]);
                    delay(1000); // give some delay for the connection to settle

                    // Sync File Start
                    sync_send();

                    return; // Exit the function if successfully connected
                }
            }
        }
    }

    // if it reached this point.
    // it means no network has been connected
    sync_stop();

    //
    app["sync_error"] = "NOT ABLE TO CONNECT TO WIFI";
    app["sync_state"] = SYNC_ERROR;
    app["clear"] = true;
    //
}

void sync_stop()
{
    WiFi.disconnect();
}

bool sync_connect_wifi(JsonDocument &app, const char *ssid, const char *password)
{
    // Connect to WiFi
    delay(1000);

    //
    String message = format("trying to connect to %s ", ssid);
    app["sync_message"] = message;
    app["clear"] = true;

    //
    WiFi.begin(ssid, password);
    delay(1000);

    int attempt = 0;
    while (WiFi.status() != WL_CONNECTED && attempt < 100)
    {
        delay(100);
        message += ".";
        app["sync_message"] = message;
        app["clear"] = true;
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
        app["sync_message"] = format("Connected - %s, %s\n", ssid, WiFi.localIP().toString().c_str());
        app["clear"] = true;

        return true;
    }
    else
    {
        _log("Failed to connect to WiFi. Please check your credentials.\n");

        //
        app["sync_error"] = format("Failed to connect to %s", ssid);
        app["sync_state"] = SYNC_ERROR;
        app["clear"] = true;

        //
        sync_stop();

        return false; // Failed to connect
    }
}

void sync_send()
{
    _log("[sync_send] Sync Send\n");

    //
    JsonDocument &app = status();

    // preparing the file
    app["sync_message"] = "Preparing the file ... ";
    app["clear"] = true;

    // Get the input file
    File inputFile = gfs()->open(Editor::getInstance().fileName.c_str(), "r");
    if (!inputFile)
    {
        app["sync_error"] = "Error opening input file!\n";
        app["sync_state"] = SYNC_ERROR;
        app["clear"] = true;

        _log(app["sync_error"]);

        sync_stop();
        return;
    }

    // Check file size
    if (inputFile.size() == 0)
    {
        app["sync_error"] = "File is empty. Sync stopped.\n";
        app["sync_state"] = SYNC_ERROR;
        app["clear"] = true;

        _log(app["sync_error"]);

        sync_stop();
        return;
    }

    // convert to BASE64 file
    String base64Filename = Editor::getInstance().fileName + ".base64";
    File outputFile = SD.open(base64Filename, FILE_WRITE);
    if (!outputFile)
    {
        //
        inputFile.close();
        delay(100);

        //
        sync_stop();

        //
        app["sync_error"] = "Error opening input file!\n";
        app["sync_state"] = SYNC_ERROR;
        app["clear"] = true;

        _log(app["sync_error"]);

        return;
    }

    // Convert to BASE64
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
    //
    inputFile.close();
    delay(100);
    outputFile.close();
    delay(100);

    //
    app["sync_message"] = "Sending file to drive ... ";
    app["clear"] = true;

    // send file to google drive
    String url = app["config"]["sync"]["url"].as<String>();
    if (!url.isEmpty() && url != "null")
    {
        _log("Requesting sync ... \n");
        _log("%s\n", url.c_str());

        // prepare http client
        HTTPClient http;
        http.begin(url);
        http.setFollowRedirects(HTTPC_FORCE_FOLLOW_REDIRECTS);

        // Read file in chunks and send via POST request
        File file = SD.open(base64Filename, FILE_READ);
        if (!file)
        {
            app["sync_error"] = "Failed to open file" + base64Filename;
            app["sync_state"] = SYNC_ERROR;
            app["clear"] = true;

            http.end();

            sync_stop();

            return;
        }

        //
        _log("Sending file: %s\n", base64Filename.c_str());
        _log("File size: %d bytes\n", file.size());
        http.sendRequest("POST", &file, file.size());
        _log("File sent successfully\n");

        // close http connection
        http.end();

        // Close file
        file.close();
        delay(100);

        //
        sync_stop();

        //
        app["sync_state"] = SYNC_COMPLETED;
        app["clear"] = true;
        _log("Sync State: SYNC_COMPLETED\n");
    }
    else
    {
        //
        app["sync_error"] = "SYNC URL NOT FOUND\n";
        app["sync_state"] = SYNC_ERROR;
        app["clear"] = true;

        _log(app["sync_error"]);

        //
        sync_stop();
    }
}
