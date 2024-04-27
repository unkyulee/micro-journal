#include "Wifi.h"
#include "../Menu.h"
#include "app/app.h"
#include "app/config/config.h"
#include "service/display/display.h"
#include "screens/WordProcessor/WordProcessor.h"

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

    ptft->println();
    ptft->println(" [B] BACK ");
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
                JsonArray savedAccessPoints = app["config"]["network"]["access_points"].as<JsonArray>();
                savedAccessPoints[wifi_config_index]["ssid"] = String(wifi_config_buffer);

                wifi_config_buffer[0] = '\0';
                wifi_config_buffer_pos = 0;

                //
                wifi_config_status = WIFI_CONFIG_EDIT_KEY;
            }
            else if (wifi_config_status == WIFI_CONFIG_EDIT_KEY)
            {
                // save ssid
                JsonArray savedAccessPoints = app["config"]["network"]["access_points"].as<JsonArray>();
                savedAccessPoints[wifi_config_index]["password"] = String(wifi_config_buffer);

                //
                config_save();

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

    // Load saved WiFi connection information from the app["config"]["access_points"] array
    JsonArray savedAccessPoints = app["config"]["network"]["access_points"].as<JsonArray>();

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
}

void _wifi_edit(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    //
    JsonDocument &app = app_status();

    // Load saved WiFi connection information from the app["config"]["access_points"] array
    JsonArray savedAccessPoints = app["config"]["network"]["access_points"].as<JsonArray>();

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