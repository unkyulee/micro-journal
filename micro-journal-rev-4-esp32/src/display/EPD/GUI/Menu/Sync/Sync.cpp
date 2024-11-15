#include "Sync.h"
#include "../Menu.h"
#include "app/app.h"
#include "config/config.h"
#include "display/display.h"
#include "editor/editor.h"
#include "../Wifi/Wifi.h"
#include <display/EPD/display_EPD.h>

//
#include "service/Wifi/WifiService.h"

//
void Sync_setup()
{
    // load wifi
    wifi_config_load();

    // init sync
    wifi_sync_init();
}

//
void Sync_render()
{
    //
    JsonDocument &app = app_status();

    // SYNC IN PROGRESS
    int cursorX = 10;
    int cursorY = 130;
    writeln(
        (GFXfont *)&systemFont,
        " SYNC & BACKUP",
        &cursorX, &cursorY,
        display_EPD_framebuffer());

    //
    cursorX = 20;
    cursorY += 35;
    writeln(
        (GFXfont *)&systemFont,
        format(" for %s", Editor::getInstance().fileBuffer.getFileName()).c_str(),
        &cursorX, &cursorY,
        display_EPD_framebuffer());

    // SYNC STATE
    int sync_state = app["sync_state"].as<int>();

    //
    if (sync_state == SYNC_START)
    {
        // Start Sync Process
        // 1) Connect to Wifi
        sync_start_request();

        cursorX = 20;
        cursorY += 35;
        writeln(
            (GFXfont *)&systemFont,
            "Trying to connect to WIFI ...",
            &cursorX, &cursorY,
            display_EPD_framebuffer());
    }

    //
    if (sync_state == SYNC_ERROR)
    {
        // error
        //
        ptft->setTextColor(TFT_WHITE, TFT_RED);
        ptft->println(sync_error);
        ptft->setTextColor(TFT_WHITE, TFT_BLACK);
    }
    else
        else if (sync_state == SYNC_SEND)
        {
            _sync_send(ptft, pu8f);
        }
    else if (sync_state == SYNC_COMPLETED)
    {
        // close WIFI
        _network_stop();

        //
        ptft->println(" - SYNC Completed");
    }
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
        _network_stop();

        //
        sync_state = -1;
    }

    //
    app["menu"]["state"] = MENU_HOME;
}
