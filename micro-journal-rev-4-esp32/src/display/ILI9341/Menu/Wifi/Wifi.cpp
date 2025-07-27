#include "Wifi.h"
#include "../Menu.h"
#include "app/app.h"
#include "display/display.h"
#include "../../WordProcessor/WordProcessor.h"

//
#include "service/WifiEntry/WifiEntry.h"
#include "service/Buffer/BufferService.h"

//
void Wifi_setup(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    WifiEntry_setup();
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

    // RENDER based on the screen index
    //
    JsonDocument &app = status();
    int wifi_config_status = app["wifi_config_status"].as<int>();
    if (wifi_config_status == WIFI_CONFIG_LIST)
    {
        Wifi_render_list(ptft, pu8f);
    }
    else if (wifi_config_status >= WIFI_CONFIG_EDIT_SSID)
    {
        Wifi_render_edit(ptft, pu8f);
    }
}

//
void Wifi_render_list(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    ptft->println(" CHOOSE THE ENTRY TO EDIT: ");
    ptft->println("");

    //
    JsonDocument &app = status();

    // initialize the config
    if (!app["wifi"].is<JsonObject>())
    {
        JsonObject wifi = app["wifi"].to<JsonObject>();
        app["wifi"] = wifi;
    }

    if (!app["wifi"]["access_points"].is<JsonArray>())
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

void Wifi_render_edit(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    //
    JsonDocument &app = status();
    int wifi_config_index = app["wifi_config_index"].as<int>();
    int wifi_config_status = app["wifi_config_status"].as<int>();

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

        ptft->printf("      %s", buffer_get());
        ptft->println("");
        ptft->println("");
        ptft->println(" [ENTER] NEXT ");
    }
    else if (wifi_config_status == WIFI_CONFIG_EDIT_KEY)
    {
        ptft->println(" TYPE WIFI KEY:");
        ptft->println("");

        ptft->printf("      %s", buffer_get());
        ptft->println("");
        ptft->println("");
        ptft->println(" [ENTER] SAVE ");
    }
}


//
void Wifi_keyboard(char key)
{
    //
    WifiEntry_keyboard(key);

    //
    Menu_clear();
}
