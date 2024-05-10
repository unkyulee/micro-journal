#include "BluetoothConfig.h"
#include "../Menu.h"
#include "app/app.h"
#include "app/config/config.h"
#include "service/display/display.h"
#include "screens/WordProcessor/WordProcessor.h"

//
void BluetoothConfig_setup(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    // when entering the screen
    // clear the screen
    Menu_clear();
}

//
void BluetoothConfig_render(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    ptft->setCursor(0, 30, 2);
    ptft->setTextSize(1);
    //
    ptft->setTextColor(TFT_WHITE, TFT_BLACK);
    ptft->println(" BLUETOOTH KEYBOARD SETUP");
    ptft->println("");

    //
    JsonDocument &app = app_status();
    bool enabled = app["config"]["bluetooth_enabled"].as<bool>();

    if (enabled)
    {

        ptft->setTextColor(TFT_BLACK, TFT_GREEN);
        ptft->println("ENABLED");
        ptft->setTextColor(TFT_WHITE, TFT_BLACK);
        ptft->println("");
    }

    // Bluetooth Keyboard Config
    ptft->println(" [Y] Enable Bluetooth Keyboard ");

    // Bluetooth Keyboard Config
    ptft->println(" [N] Disable Bluetooth Keyboard ");

    //
    ptft->println();
    ptft->println(" [B] BACK ");
}

//
void BluetoothConfig_keyboard(char key)
{
    JsonDocument &app = app_status();

    if (key == 'y')
    {
        //
        app["config"]["bluetooth_enabled"] = true;

        // 
        config_save();

        return;
    }
    else if (key == 'n')
    {
        //
        app["config"]["bluetooth_enabled"] = false;

        //
        config_save();

        return;
    }

    // go back to home menu
    //
    app["menu"]["state"] = MENU_HOME;
}
