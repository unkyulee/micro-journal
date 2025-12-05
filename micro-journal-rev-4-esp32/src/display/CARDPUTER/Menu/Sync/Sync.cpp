#include "Sync.h"
#include "../Menu.h"
#include "app/app.h"
#include "display/display.h"
//
#include "service/WifiEntry/WifiEntry.h"
#include "service/Sync/Sync.h"
#include "service/Editor/Editor.h"

//
#include "display/CARDPUTER/display_CARDPUTER.h"

//
void Sync_setup()
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
void Sync_render()
{
    //
    JsonDocument &app = status();
    
    // header
    // Text to be displayed
    M5Cardputer.Display.setTextSize(0.7);
    M5Cardputer.Display.setTextColor(TFT_WHITE, TFT_BLACK);
    M5Cardputer.Display.setFont(&fonts::Orbitron_Light_24);
    
    M5Cardputer.Display.drawString("SYNC & BACKUP ", 0, 0);
    
    // SYNC STATE
    int sync_state = app["sync_state"].as<int>();

    //
    if (sync_state == SYNC_START)
    {
        //
        drawWrappedText("Preparing Sync Process ...", 0, 30, M5Cardputer.Display.width());

        //
        sync_start_request();
    }

    else if (sync_state == SYNC_STARTED || sync_state == SYNC_PROGRESS)
    {
        String message = app["sync_message"].as<String>();
        if (message != "")
        {
            //
            drawWrappedText(message, 0, 30, M5Cardputer.Display.width());
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
            M5Cardputer.Display.setTextColor(TFT_WHITE, TFT_RED);
            M5Cardputer.Display.drawString(error, 0, 30);
            M5Cardputer.Display.setTextColor(TFT_WHITE, TFT_BLACK);
        }
    }

    else if (sync_state == SYNC_COMPLETED)
    {
        //
        drawWrappedText("SYNC Completed. Press any key to exit", 0, 30, M5Cardputer.Display.width());
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
