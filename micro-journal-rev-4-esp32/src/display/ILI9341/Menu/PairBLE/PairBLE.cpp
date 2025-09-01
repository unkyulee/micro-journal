#include "PairBLE.h"

//
#include "../Menu.h"
#include "app/app.h"
#include "keyboard/keyboard.h"
#include "display/display.h"

//
#include "service/BLEServer/BLEServer.h"

//
void PairBLE_setup(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    //
    Menu_clear();

    //
    JsonDocument &app = status();

    // Check if already connected device exists
    if (app["config"]["ble"]["name"].is<const char *>())
    {
        // When it is already paired then show UNPAIR
    }
    else
    {
        // Start Scanning
        BLEServer_setup("Micro Journal 5");
    }
}

//
void PairBLE_render(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    // Text to be displayed
    ptft->setCursor(0, 30, 2);
    ptft->setTextSize(1);

    //
    ptft->setTextColor(TFT_WHITE, TFT_BLACK);
    ptft->println("Pairing BLE Keyboard ");
    ptft->println("");

    // RENDER based on the screen index
    //
    JsonDocument &app = status();

    // UNPAIR

    // Check if already connected device exists
    if (app["config"]["ble"]["name"].is<const char *>())
    {
        const char *name = app["config"]["ble"]["name"].as<const char *>();
        ptft->printf("[MENU, M] UNPAIR - %s", name);
        ptft->println("");
    }
    else
    {
        //
        int ble_status = app["ble_state"].as<int>();
        _debug("[PairBLE_render] ble_state %d\n", ble_status);
        if (ble_status == BLE_CONFIG_LIST || ble_status == BLE_CONFIG_NO_DEVICES)
        {
            // display the connected devices
            PairBLE_render_list(ptft, pu8f);
        }
    }

    // BACK
    ptft->println();
    ptft->println("[B] BACK ");
}

//
void PairBLE_render_list(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    //
    JsonDocument &app = status();

    // retrieve ble keyboard devices
    JsonArray devices = app["ble_devices"].as<JsonArray>();

    // if BLE_CONFIG_NO_DEVICES
    int ble_state = app["ble_state"].as<int>();
    _debug("[PairBLE_render_list] ble_state %d\n", ble_state);
    if (ble_state == BLE_CONFIG_NO_DEVICES)
    {
        ptft->println("No Devices Found");
    }
    else if (devices.size() == 0)
    {
        ptft->println("Scanning ...");
    }
    else if (devices.size() > 0)
    {
        // printing devices
        _log("Rendering device list: %d\n", devices.size());
        ptft->println("Press number to pair with");
        for (int i = 0; i < devices.size(); i++)
        {
            if (i == 0)
            {
                const char *name = devices[i]["name"].as<const char *>();
                ptft->printf("  [MENU, %d] %s\n", i + 1, name);
            }
            else
            {
                const char *name = devices[i]["name"].as<const char *>();
                ptft->printf("  [%d] %s\n", i + 1, name);
            }
        }
        ptft->println("");
    }
}

//
void PairBLE_keyboard(char key)
{
    //
    JsonDocument &app = status();

    //
    BLEServer_keyboard(key);

    //
    Menu_clear();

    // UNPAIR
    if (app["config"]["ble"]["name"].is<const char *>())
    {
        if (key == 'd' || key == MENU)
        {
            //
            if (app["config"]["ble"].is<JsonObject>())
            {
                //
                app["config"].remove("ble");

                // save config
                config_save();

                // restart
                ESP.restart();
            }
        }
    }

    //
    if ((key >= '1' && key <= '9') || key == MENU)
    {
        // when menu is pressed then chose the first keyboard
        if (key == MENU)
            key = '1';

        // device is chosen

        // retrieve ble keyboard devices
        JsonArray devices = app["ble_devices"].as<JsonArray>();

        int selectedIndex = key - '1';
        if (devices.size() > selectedIndex)
        {
            // save the device id
            app["config"]["ble"]["address"] = devices[selectedIndex]["address"];
            app["config"]["ble"]["name"] = devices[selectedIndex]["name"];
            app["config"]["ble"]["type"] = devices[selectedIndex]["type"];

            //
            _log("Saving BLE keyboard: address: %s name: %s type: %d\n",
                 app["config"]["ble"]["address"].as<const char *>(),
                 app["config"]["ble"]["name"].as<const char *>(),
                 app["config"]["ble"]["type"].as<int>());

            /// save config
            config_save();

            // restart
            ESP.restart();
        }
    }
}
