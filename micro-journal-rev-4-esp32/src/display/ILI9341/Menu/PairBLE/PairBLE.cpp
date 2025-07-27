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

    // Initialize BLEServer
    BLEServer_setup();
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
    int ble_status = app["ble_state"].as<int>();
    _debug("[PairBLE_render] ble_state %d\n", ble_status);
    if (ble_status == BLE_CONFIG_LIST || ble_status == BLE_CONFIG_NO_DEVICES)
    {
        // display the connected devices
        PairBLE_render_list(ptft, pu8f);
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
    JsonArray devices = app["ble"]["devices"].as<JsonArray>();

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
        // Iterate through each available network
        for (int i = 0; i < devices.size(); i++)
        {
            // available wifi network
            const char *name = devices[i]["name"].as<const char *>();
            ptft->printf(" [%d] %s\n", i + 1, name);
        }
    }
}

//
void PairBLE_keyboard(char key)
{
    //
    BLEServer_keyboard(key);

    //
    Menu_clear();
}
