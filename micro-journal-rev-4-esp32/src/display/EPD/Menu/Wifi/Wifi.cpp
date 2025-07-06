#include "Wifi.h"
#include "../Menu.h"
#include "app/app.h"
#include "display/display.h"
#include <display/EPD/display_EPD.h>

//
#include "service/WifiEntry/WifiEntry.h"
#include "service/Buffer/BufferService.h"

//
void Wifi_setup()
{
    WifiEntry_setup();    
}

//
void Wifi_render()
{
    //
    JsonDocument &app = status();

    // WIFI SETTINGS
    int cursorX = 10;
    int cursorY = 80;
    writeln(
        (GFXfont *)&systemFont,
        " WIFI SETTINGS ",
        &cursorX, &cursorY,
        display_EPD_framebuffer());

    // RENDER based on the screen index
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
void Wifi_keyboard(char key)
{
    WifiEntry_keyboard(key);
}

//
void Wifi_render_list()
{
    //
    JsonDocument &app = status();

    // List of WIFI entries
    int cursorX = 10;
    int cursorY = 130;
    writeln(
        (GFXfont *)&systemFont,
        " CHOOSE THE ENTRY TO EDIT: ",
        &cursorX, &cursorY,
        display_EPD_framebuffer());

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
    for (int i = 0; i < 5; i++)
    {
        // new line
        cursorX = 10;
        cursorY += 35;

        //
        if (savedAccessPoints.size() > i)
        {
            // available wifi network
            const char *savedSsid = savedAccessPoints[i]["ssid"].as<const char *>();
            writeln(
                (GFXfont *)&systemFont,
                format(" [%d] %s\n", i + 1, savedSsid).c_str(),
                &cursorX, &cursorY,
                display_EPD_framebuffer());
        }
        else
        {
            writeln(
                (GFXfont *)&systemFont,
                format(" [%d]", i + 1).c_str(),
                &cursorX, &cursorY,
                display_EPD_framebuffer());
        }
    }

    cursorX = 10;
    cursorY += 50;
    writeln(
        (GFXfont *)&systemFont,
        " [B] Back",
        &cursorX, &cursorY,
        display_EPD_framebuffer());
}

//
void Wifi_render_edit()
{
    // EDIT WIFI CONFIG
    JsonDocument &app = status();
    int wifi_config_index = app["wifi_config_index"].as<int>();
    int wifi_config_status = app["wifi_config_status"].as<int>();

    // Load saved WiFi connection information from the app["config"]["access_points"] array
    JsonArray savedAccessPoints = app["wifi"]["access_points"].as<JsonArray>();

    const char *savedSsid = savedAccessPoints[wifi_config_index]["ssid"];
    const char *savedPassword = savedAccessPoints[wifi_config_index]["password"];

    //
    int cursorX = 10;
    int cursorY = 130;
    writeln(
        (GFXfont *)&systemFont,
        format(" EDIT [%d] WIFI CONFIG", wifi_config_index + 1).c_str(),
        &cursorX, &cursorY,
        display_EPD_framebuffer());

    if (wifi_config_status == WIFI_CONFIG_EDIT_SSID)
    {
        cursorX = 10;
        cursorY += 50;
        writeln(
            (GFXfont *)&systemFont,
            " TYPE SSID:",
            &cursorX, &cursorY,
            display_EPD_framebuffer());

        // show what is in the buffer
        cursorX = 50;
        cursorY += 50;
        writeln(
            (GFXfont *)&monospace,
            buffer_get(),
            &cursorX, &cursorY,
            display_EPD_framebuffer());

        cursorX = 10;
        cursorY += 50;
        writeln(
            (GFXfont *)&systemFont,
            " [ENTER] NEXT ",
            &cursorX, &cursorY,
            display_EPD_framebuffer());
    }
    else if (wifi_config_status == WIFI_CONFIG_EDIT_KEY)
    {
        cursorX = 10;
        cursorY += 50;
        writeln(
            (GFXfont *)&systemFont,
            " TYPE WIFI KEY:",
            &cursorX, &cursorY,
            display_EPD_framebuffer());

        // show what is in the buffer
        cursorX = 50;
        cursorY += 50;
        writeln(
            (GFXfont *)&monospace,
            buffer_get(),
            &cursorX, &cursorY,
            display_EPD_framebuffer());

        cursorX = 10;
        cursorY += 50;
        writeln(
            (GFXfont *)&systemFont,
            " [ENTER] SAVE ",
            &cursorX, &cursorY,
            display_EPD_framebuffer());
    }
}