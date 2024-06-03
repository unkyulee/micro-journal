#include "Wifi.h"
#include "../Menu.h"
#include "app/app.h"
#include "app/config/config.h"
#include "service/display/display.h"
#include "screens/WordProcessor/WordProcessor.h"

//
#include <SPI.h>
#include <SPIFFS.h>
#include <SD.h>

//
#define WIFI_CONFIG_LIST 0
#define WIFI_CONFIG_EDIT_SSID 1
#define WIFI_CONFIG_EDIT_KEY 2
int wifi_config_status = 0;
int wifi_config_index = 0;

#define WIFI_CONFIG_BUFFER 256
char wifi_config_buffer[WIFI_CONFIG_BUFFER];
int wifi_config_buffer_pos = 0;

void _wifi_saved_list(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f);
void _wifi_edit(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f);

//
void Wifi_setup(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    //
    Menu_clear();

    // 
    Wifi_load();

    //
    wifi_config_status = WIFI_CONFIG_LIST;

    // set wifi entry as 5
    JsonDocument &app = app_status();
    JsonArray savedAccessPoints = app["config"]["network"]["access_points"].as<JsonArray>();

    // Iterate through each available network
    bool needSave = false;
    for (int i = 0; i < 5; i++)
    {
        if (savedAccessPoints.size() > i)
        {
        }
        else
        {
            needSave = true;

            savedAccessPoints[i]["ssid"] = "";
            savedAccessPoints[i]["password"] = "";
        }
    }

    //
    if (needSave)
    {
        app_log("Wifi settings needs saving\n");
        config_save();
    }
}

//
void Wifi_render(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    // Text to be displayed
    ptft->setCursor(0, 30, 2);
    ptft->setTextSize(1);

    //
    ptft->setTextColor(TFT_WHITE, TFT_BLACK);
    ptft->println(" WIFI SETTINGS ");
    ptft->println("");

    if (wifi_config_status == WIFI_CONFIG_LIST)
    {
        _wifi_saved_list(ptft, pu8f);
    }
    else if (wifi_config_status >= WIFI_CONFIG_EDIT_SSID)
    {
        _wifi_edit(ptft, pu8f);
    }
}

//
void Wifi_keyboard(char key)
{
    JsonDocument &app = app_status();

    if (wifi_config_status >= WIFI_CONFIG_EDIT_SSID)
    {
        // BACK SPACE
        if (key == '\b')
        {
            // backspace
            wifi_config_buffer[wifi_config_buffer_pos--] = '\0';
        }
        else if (key == '\n')
        {
            // next
            if (wifi_config_status == WIFI_CONFIG_EDIT_SSID)
            {
                // save ssid
                JsonArray savedAccessPoints = app["wifi"]["access_points"].as<JsonArray>();
                savedAccessPoints[wifi_config_index]["ssid"] = String(wifi_config_buffer);

                wifi_config_buffer[0] = '\0';
                wifi_config_buffer_pos = 0;

                //
                wifi_config_status = WIFI_CONFIG_EDIT_KEY;
            }
            else if (wifi_config_status == WIFI_CONFIG_EDIT_KEY)
            {
                // save ssid
                JsonArray savedAccessPoints = app["wifi"]["access_points"].as<JsonArray>();
                savedAccessPoints[wifi_config_index]["password"] = String(wifi_config_buffer);

                //
                Wifi_save();

                wifi_config_buffer[0] = '\0';
                wifi_config_buffer_pos = 0;

                //
                wifi_config_status = WIFI_CONFIG_LIST;
            }
        }
        else
        {
            // edit mode
            wifi_config_buffer[wifi_config_buffer_pos++] = key;
            wifi_config_buffer[wifi_config_buffer_pos] = '\0';
        }

        // OVERFLOW CONTROL
        if (wifi_config_buffer_pos > WIFI_CONFIG_BUFFER)
        {
            wifi_config_buffer[0] = '\0';
            wifi_config_buffer_pos = 0;
        }
    }
    else
    {
        // back to home
        if (key == 'B' || key == 'b')
        {
            //
            // go back to home menu
            app["menu"]["state"] = MENU_HOME;

            return;
        }

        else if (key >= '1' && key <= '5')
        {
            //
            // wifi entry chose to edit
            wifi_config_index = key - '1';
            wifi_config_status = WIFI_CONFIG_EDIT_SSID;
            wifi_config_buffer[0] = '\0';
            wifi_config_buffer_pos = 0;

            //
            Menu_clear();

            return;
        }
    }
}

void _wifi_saved_list(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    ptft->println(" CHOOSE THE ENTRY TO EDIT: ");
    ptft->println("");

    //
    JsonDocument &app = app_status();

    // initialize the config
    if (!app.containsKey("wifi"))
    {
        JsonObject wifi = app["wifi"].to<JsonObject>();
        app["wifi"] = wifi;
    }

    if (!app["wifi"].containsKey("access_points"))
    {
        JsonArray access_points = app["wifi"]["access_points"].to<JsonArray>();
        app["wifi"]["access_points"] = access_points;
    }

    // Load saved WiFi connection information from the app["config"]["access_points"] array
    JsonArray savedAccessPoints = app["wifi"]["access_points"].as<JsonArray>();

    // Iterate through each available network
    for (int i = 0; i < 5; i++)
    {
        if (savedAccessPoints.size() > i)
        {
            // available wifi network
            const char *savedSsid = savedAccessPoints[i]["ssid"].as<const char *>();
            ptft->printf(" [%d] %s\n", i + 1, savedSsid);
        }
        else
        {
            ptft->printf(" [%d]\n", i + 1);
        }
    }

    ptft->println();
    ptft->println(" [B] BACK ");
}

void _wifi_edit(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    //
    JsonDocument &app = app_status();

    // Load saved WiFi connection information from the app["config"]["access_points"] array
    JsonArray savedAccessPoints = app["wifi"]["access_points"].as<JsonArray>();

    const char *savedSsid = savedAccessPoints[wifi_config_index]["ssid"];
    const char *savedPassword = savedAccessPoints[wifi_config_index]["password"];

    ptft->printf(" EDIT [%d] WIFI CONFIG", wifi_config_index + 1);
    ptft->println("");

    if (wifi_config_status == WIFI_CONFIG_EDIT_SSID)
    {
        ptft->println(" TYPE SSID:");
        ptft->println("");

        ptft->printf("      %s", wifi_config_buffer);
        ptft->println("");
        ptft->println("");
        ptft->println(" [ENTER] NEXT ");
    }
    else if (wifi_config_status == WIFI_CONFIG_EDIT_KEY)
    {
        ptft->println(" TYPE WIFI KEY:");
        ptft->println("");

        ptft->printf("      %s", wifi_config_buffer);
        ptft->println("");
        ptft->println("");
        ptft->println(" [ENTER] SAVE ");
    }
}

void Wifi_load()
{
    //
    JsonDocument &app = app_status();

    // load config.json
    app_log("Opening wifi.json file\n");
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

            return;
        }

        // Assign the loaded configuration to "config" property of app
        app_log("Loading app status config\n");
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

void Wifi_save()
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
    if (app.containsKey("wifi"))
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