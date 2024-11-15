#include "Wifi.h"
#include "../Menu.h"
#include "app/app.h"
#include "config/config.h"
#include "display/display.h"
#include <display/EPD/display_EPD.h>

// services
#include "service/Buffer/BufferService.h"
#include "service/Wifi/WifiService.h"

//
#define WIFI_CONFIG_LIST 0
#define WIFI_CONFIG_EDIT_SSID 1
#define WIFI_CONFIG_EDIT_KEY 2

//
int wifi_config_status = 0;
int wifi_config_index = 0;

//
void Wifi_setup()
{
    // load wifi configuration from SPIFF
    wifi_config_load();

    // set the screen to show the wifi configuration list
    wifi_config_status = WIFI_CONFIG_LIST;
}

//
void Wifi_render()
{
    // WIFI SETTINGS
    int cursorX = 10;
    int cursorY = 80;
    writeln(
        (GFXfont *)&systemFont,
        " WIFI SETTINGS ",
        &cursorX, &cursorY,
        display_EPD_framebuffer());

    // RENDER based on the screen index
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
    //
    JsonDocument &app = app_status();

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

void Wifi_render_edit()
{
    // EDIT WIFI CONFIG
    JsonDocument &app = app_status();

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
            (GFXfont *)&systemFont,
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
            (GFXfont *)&systemFont,
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

//
void Wifi_keyboard(char key)
{
    JsonDocument &app = app_status();

    if (wifi_config_status >= WIFI_CONFIG_EDIT_SSID)
    {

        // SAVE or NEXT
        if (key == '\n')
        {
            // NEXT step
            if (wifi_config_status == WIFI_CONFIG_EDIT_SSID)
            {
                // save ssid
                JsonArray savedAccessPoints = app["wifi"]["access_points"].as<JsonArray>();
                savedAccessPoints[wifi_config_index]["ssid"] = String(buffer_get());

                // clear buffer
                buffer_clear();

                // move to password enter screen
                wifi_config_status = WIFI_CONFIG_EDIT_KEY;
            }
            else if (wifi_config_status == WIFI_CONFIG_EDIT_KEY)
            {
                // save ssid
                JsonArray savedAccessPoints = app["wifi"]["access_points"].as<JsonArray>();
                savedAccessPoints[wifi_config_index]["password"] = String(buffer_get());

                // save the configuration
                wifi_config_save();

                //
                buffer_clear();

                // go back to the configuration list
                wifi_config_status = WIFI_CONFIG_LIST;
            }
        }
        // BACK SPACE
        else if (key == '\b')
        {
            // backspace
            buffer_remove();
        }
        // ADD KEYS
        else
        {
            // edit mode
            buffer_add(key);
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
            //

            // clear buffer
            buffer_clear();

            // determine the index to edit
            wifi_config_index = key - '1';

            // move the screen
            wifi_config_status = WIFI_CONFIG_EDIT_SSID;

            return;
        }
    }
}
