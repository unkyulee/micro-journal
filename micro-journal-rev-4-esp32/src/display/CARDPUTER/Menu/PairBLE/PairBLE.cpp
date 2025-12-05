#include "PairBLE.h"

//
#include "../Menu.h"
#include "app/app.h"
#include "keyboard/keyboard.h"
#include "display/display.h"

//
#include "service/BLEServer/BLEServer.h"

//
#include "display/CARDPUTER/display_CARDPUTER.h"

//
void PairBLE_setup()
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
        BLEServer_setup("MJ HOST");
    }
}

//
void PairBLE_render()
{
    JsonDocument &app = status();
    M5Cardputer.Display.setTextSize(0.5);
    M5Cardputer.Display.setTextColor(TFT_WHITE, TFT_BLACK);
    M5Cardputer.Display.setFont(&fonts::Orbitron_Light_24);

    const int lineHeight = M5Cardputer.Display.fontHeight() + 4;
    const int contentStartY = lineHeight + 2;

    M5Cardputer.Display.drawString("PAIRING BLE KEYBOARD", 0, 0);

    int y = contentStartY;
    // Check if already connected device exists and offer unpair
    if (app["config"]["ble"]["name"].is<const char *>())
    {
        const char *name = app["config"]["ble"]["name"].as<const char *>();
        String message = "[M] UNPAIR - ";
        message += name;
        M5Cardputer.Display.drawString(message, 0, y);
        y += lineHeight;
    }
    else
    {
        int ble_status = app["ble_state"].as<int>();
        _debug("[PairBLE_render] ble_state %d\n", ble_status);
        if (ble_status == BLE_CONFIG_LIST || ble_status == BLE_CONFIG_NO_DEVICES)
        {
            // display the connected devices
            PairBLE_render_list(contentStartY);
        }
    }

    // BACK button hint near the bottom
    int footerY = M5Cardputer.Display.height() - M5Cardputer.Display.fontHeight() - 2;
    if (footerY < y)
    {
        footerY = y;
    }
    M5Cardputer.Display.drawString("[B] BACK", 0, footerY);
}

//
void PairBLE_render_list(int startY)
{
    //
    JsonDocument &app = status();

    // retrieve ble keyboard devices
    JsonArray devices = app["ble_devices"].as<JsonArray>();

    // if BLE_CONFIG_NO_DEVICES
    int ble_state = app["ble_state"].as<int>();
    _debug("[PairBLE_render_list] ble_state %d\n", ble_state);
    int y = startY;
    if (ble_state == BLE_CONFIG_NO_DEVICES)
    {
        M5Cardputer.Display.drawString("No Devices Found", 0, y);
    }
    else if (devices.size() == 0)
    {
        M5Cardputer.Display.drawString("Scanning ...", 0, y);
    }
    else if (devices.size() > 0)
    {
        // printing devices
        _log("Rendering device list: %d\n", devices.size());
        M5Cardputer.Display.drawString("Press number to pair with", 0, y);
        y += M5Cardputer.Display.fontHeight() + 4;
        for (int i = 0; i < devices.size(); i++)
        {
            const char *name = devices[i]["name"].as<const char *>();
            String line;
            if (i == 0)
            {
                line = "  [" + String(i + 1) + "] " + String(name);
            }
            else
            {
                line = "  [" + String(i + 1) + "] " + String(name);
            }
            M5Cardputer.Display.drawString(line, 0, y);
            y += M5Cardputer.Display.fontHeight() + 2;
        }
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
        if (key == 'm' || key == MENU)
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
