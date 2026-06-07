#include "Wifi.h"
#include "../Menu.h"
#include "app/app.h"
#include "display/display.h"
#include "../../WordProcessor/WordProcessor.h"

//
#include "service/WifiEntry/WifiEntry.h"
#include "service/Buffer/BufferService.h"

//
void Wifi_setup(ST7305_4p2_BW_DisplayDriver *display, U8G2_FOR_ST73XX *u8)
{
    WifiEntry_setup();
}

//
void Wifi_render(ST7305_4p2_BW_DisplayDriver *display, U8G2_FOR_ST73XX *u8)
{
    // Text to be displayed
    u8->setCursor(0, 50);

    //
    u8->println(" WIFI SETTINGS ");
    u8->println("");

    // RENDER based on the screen index
    //
    JsonDocument &app = status();
    int wifi_config_status = app["wifi_config_status"].as<int>();
    if (wifi_config_status == WIFI_CONFIG_LIST)
    {
        Wifi_render_list(display, u8);
    }
    else if (wifi_config_status >= WIFI_CONFIG_EDIT_SSID)
    {
        Wifi_render_edit(display, u8);
    }
}

//
void Wifi_render_list(ST7305_4p2_BW_DisplayDriver *display, U8G2_FOR_ST73XX *u8)
{
    u8->println(" CHOOSE THE ENTRY TO EDIT: ");
    u8->println("");

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
            u8->printf(" [%d] %s\n", i + 1, savedSsid);
        }
        else
        {
            u8->printf(" [%d]\n", i + 1);
        }
    }

    u8->println();
    u8->println(" [B] BACK ");
}

void Wifi_render_edit(ST7305_4p2_BW_DisplayDriver *display, U8G2_FOR_ST73XX *u8)
{
    //
    JsonDocument &app = status();
    int wifi_config_index = app["wifi_config_index"].as<int>();
    int wifi_config_status = app["wifi_config_status"].as<int>();

    // Load saved WiFi connection information from the app["config"]["access_points"] array
    JsonArray savedAccessPoints = app["wifi"]["access_points"].as<JsonArray>();

    const char *savedSsid = savedAccessPoints[wifi_config_index]["ssid"];
    const char *savedPassword = savedAccessPoints[wifi_config_index]["password"];

    u8->printf(" EDIT [%d] WIFI CONFIG", wifi_config_index + 1);
    u8->println("");

    if (wifi_config_status == WIFI_CONFIG_EDIT_SSID)
    {
        u8->println(" TYPE SSID:");
        u8->println("");

        u8->printf("      %s", buffer_get());
        u8->println("");
        u8->println("");
        u8->println(" [ENTER] NEXT ");
    }
    else if (wifi_config_status == WIFI_CONFIG_EDIT_KEY)
    {
        u8->println(" TYPE WIFI KEY:");
        u8->println("");

        u8->printf("      %s", buffer_get());
        u8->println("");
        u8->println("");
        u8->println(" [ENTER] SAVE ");
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
