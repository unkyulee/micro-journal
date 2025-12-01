#include "Wifi.h"
#include "../Menu.h"
#include "app/app.h"
#include "display/display.h"
#include "../../WordProcessor/WordProcessor.h"

//
#include "service/WifiEntry/WifiEntry.h"
#include "service/Buffer/BufferService.h"

//
#include "display/CARDPUTER/display_CARDPUTER.h"

//
void Wifi_setup()
{
    WifiEntry_setup();
}

//
void Wifi_render()
{
    // Text to be displayed
    M5Cardputer.Display.setTextSize(0.5);
    M5Cardputer.Display.setTextColor(TFT_WHITE, TFT_BLACK);
    M5Cardputer.Display.setFont(&fonts::Orbitron_Light_24);

    // RENDER based on the screen index
    //
    JsonDocument &app = status();
    int wifi_config_status = app["wifi_config_status"].as<int>();
    if (wifi_config_status == WIFI_CONFIG_LIST)
    {
        Wifi_render_list();
    }
    else if (wifi_config_status >= WIFI_CONFIG_EDIT_SSID)
    {
        Wifi_render_edit();
    }
}

//
void Wifi_render_list()
{
    M5Cardputer.Display.drawString(" CHOOSE THE ENTRY TO EDIT: ", 0, 0);

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
    int y = 20;
    for (int i = 0; i < 5; i++)
    {
        if (savedAccessPoints.size() > i)
        {
            // available wifi network
            const char *savedSsid = savedAccessPoints[i]["ssid"].as<const char *>();
            M5Cardputer.Display.drawString(format(" [%d] %s", i + 1, savedSsid), 0, y);
        }
        else
        {
            M5Cardputer.Display.drawString(format(" [%d]", i + 1), 0, y);
        }

        y += 18;
    }

    M5Cardputer.Display.drawString(" [B] BACK ", 0, y);
}

void Wifi_render_edit()
{
    //
    JsonDocument &app = status();
    int wifi_config_index = app["wifi_config_index"].as<int>();
    int wifi_config_status = app["wifi_config_status"].as<int>();

    // Load saved WiFi connection information from the app["config"]["access_points"] array
    JsonArray savedAccessPoints = app["wifi"]["access_points"].as<JsonArray>();

    const char *savedSsid = savedAccessPoints[wifi_config_index]["ssid"];
    const char *savedPassword = savedAccessPoints[wifi_config_index]["password"];

    int y = 0;
    M5Cardputer.Display.drawString(format(" EDIT [%d] WIFI CONFIG", wifi_config_index + 1), 0, y);

    if (wifi_config_status == WIFI_CONFIG_EDIT_SSID)
    {
        y += 20;
        M5Cardputer.Display.drawString(" TYPE SSID:", 0, y);

        y += 20;
        M5Cardputer.Display.drawString(format("      %s", buffer_get()), 0, y);

        y += 60;
        M5Cardputer.Display.drawString(" [ENTER] NEXT ", 0, y);

    }
    else if (wifi_config_status == WIFI_CONFIG_EDIT_KEY)
    {
        y += 20;
        M5Cardputer.Display.drawString(" TYPE WIFI KEY:", 0, y);

        y += 20;
        M5Cardputer.Display.drawString(format("      %s", buffer_get()), 0, y);

        y += 60;
        M5Cardputer.Display.drawString(" [ENTER] SAVE ", 0, y);
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
