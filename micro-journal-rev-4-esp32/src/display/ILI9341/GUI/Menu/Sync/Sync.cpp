#include "Sync.h"
#include "../Menu.h"
#include "app/app.h"
#include "config/config.h"
#include "display/display.h"
#include "editor/editor.h"
#include "../Wifi/Wifi.h"

//
#include "service/Wifi/WifiService.h"

//
void Sync_setup(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    // load wifi
    wifi_config_load();

    // init sync
    wifi_sync_init();
}

//
void Sync_render(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    //
    JsonDocument &app = app_status();

    // header
    ptft->setCursor(0, 30, 2);
    ptft->setTextColor(TFT_WHITE, TFT_BLACK);
    ptft->println(" SYNC & BACKUP ");

    // SYNC STATE
    int sync_state = app["sync_state"].as<int>();

    //
    if (sync_state == SYNC_START)
    {
        // Start Sync Process
        // 1) Connect to Wifi
        sync_start_request();
        ptft->println(" - PREPREPARING ");

        String message = app["sync_message"].as<String>();

        if (message != "")
        {
            ptft->println(message);
        }
    }
    //
    // error
    //
    else if (sync_state == SYNC_ERROR)
    {
        String error = app["sync_error"].as<String>();
        if (error != "")
        {
            ptft->setTextColor(TFT_WHITE, TFT_RED);
            ptft->println(error);
            ptft->setTextColor(TFT_WHITE, TFT_BLACK);
        }
    }

    else if (sync_state == SYNC_COMPLETED)
    {
        //
        ptft->println(" - SYNC Completed");
    }

    ptft->println("");
    ptft->println(" press [B] to exit ");
}

//
void Sync_keyboard(char key)
{
    //
    JsonDocument &app = app_status();

    // when pressed B
    if (key == '\b' || key == 'b' || key == 'B')
    {
        // stop network
        sync_stop();

        //
        app["sync_state"] = -1;

        //
        app["menu"]["state"] = MENU_HOME;
    }
}
