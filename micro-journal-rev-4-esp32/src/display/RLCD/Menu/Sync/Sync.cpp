#include "Sync.h"
#include "../Menu.h"
#include "app/app.h"
#include "display/display.h"
//
#include "service/WifiEntry/WifiEntry.h"
#include "service/Sync/Sync.h"
#include "service/Editor/Editor.h"

//
void Sync_setup(ST7305_4p2_BW_DisplayDriver *display, U8G2_FOR_ST73XX *u8)
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
void Sync_render(ST7305_4p2_BW_DisplayDriver *display, U8G2_FOR_ST73XX *u8)
{
    //
    JsonDocument &app = status();

    // header
    u8->setCursor(0, 50);
    u8->println(" SYNC & BACKUP ");
    u8->println("");

    // SYNC STATE
    int sync_state = app["sync_state"].as<int>();

    //
    if (sync_state == SYNC_START)
    {
        //
        u8->println("Preparing Sync Process ...");

        //
        sync_start_request();
    }

    else if (sync_state == SYNC_STARTED || sync_state == SYNC_PROGRESS)
    {
        String message = app["sync_message"].as<String>();
        if (message != "")
        {
            //
            u8->println(message);
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
            u8->println(error);
        }
    }

    else if (sync_state == SYNC_COMPLETED)
    {
        //
        u8->println("SYNC Completed. Press any key to exit");
    }
}

//
void Sync_keyboard(char key)
{
    //
    JsonDocument &app = status();

    // SYNC STATE
    int sync_state = app["sync_state"].as<int>();
    if (sync_state == SYNC_COMPLETED || sync_state == SYNC_ERROR)
    {
        app["screen"] = WORDPROCESSOR;
    }
}
