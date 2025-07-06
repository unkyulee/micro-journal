#include "Sync.h"
#include "../Menu.h"
#include "app/app.h"
#include "display/display.h"
//
#include "service/WifiEntry/WifiEntry.h"
#include "service/Sync/Sync.h"
#include "service/Editor/Editor.h"

//
void Sync_setup(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    _log("Sync_setup\n");

    //
    Menu_clear();

    // load wifi
    wifi_config_load();

    // init sync
    sync_init();
}

//
void Sync_render(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    //
    JsonDocument &app = status();

    // header
    ptft->setCursor(0, 30, 2);
    ptft->setTextColor(TFT_WHITE, TFT_BLACK);
    ptft->println(" SYNC & BACKUP ");
    ptft->println("");

    // SYNC STATE
    int sync_state = app["sync_state"].as<int>();

    //
    if (sync_state == SYNC_START)
    {
        //
        ptft->println("Preparing Sync Process ...");

        //
        sync_start_request();
    }

    else if (sync_state == SYNC_STARTED || sync_state == SYNC_PROGRESS)
    {
        String message = app["sync_message"].as<String>();
        if (message != "")
        {
            //
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
        ptft->println("SYNC Completed. Press any key to exit");
    }
}

//
void Sync_keyboard(char key)
{
    //
    JsonDocument &app = status();

    // SYNC STATE
    int sync_state = app["sync_state"].as<int>();
    if (sync_state == SYNC_COMPLETED)
    {
        app["screen"] = WORDPROCESSOR;
    }
}
