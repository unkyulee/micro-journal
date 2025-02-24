#include "Wifi.h"
#include "../Menu.h"
#include "app/app.h"
#include "config/config.h"
#include "display/display.h"
#include "../../WordProcessor/WordProcessor.h"

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
void Wifi_setup(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    //
    Menu_clear();

    // load wifi configuration from SPIFF
    wifi_config_load();

    //
    wifi_config_status = WIFI_CONFIG_LIST;
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
    if (wifi_config_status == WIFI_CONFIG_LIST)
    {
        Wifi_render_list(ptft, pu8f);
    }
    else if (wifi_config_status >= WIFI_CONFIG_EDIT_SSID)
    {
        Wifi_render_edit(ptft, pu8f);
    }
}

void Wifi_render_list(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    ptft->println(" CHOOSE THE ENTRY TO EDIT: ");
    ptft->println("");

    //
    JsonDocument &app = app_status();

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
