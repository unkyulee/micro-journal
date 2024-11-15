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
    int cursorY = 120;
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

        String message = app["sync_message"].as<String>();

        if (message != "")
        {
            cursorX = 30;
            cursorY += 50;
            writeln(
                (GFXfont *)&systemFont,
                message.c_str(),
                &cursorX, &cursorY,
                display_EPD_framebuffer());
        }
    }

    //
    else if (sync_state == SYNC_ERROR)
    {
        String error = app["sync_error"].as<String>();
        if (error != "")
        {
            cursorX = 30;
            cursorY += 50;
            writeln(
                (GFXfont *)&systemFont,
                error.c_str(),
                &cursorX, &cursorY,
                display_EPD_framebuffer());
        }
    }

    else if (sync_state == SYNC_COMPLETED)
    {
        //
        cursorX = 30;
        cursorY += 35;
        writeln(
            (GFXfont *)&systemFont,
            "SYNC Completed",
            &cursorX, &cursorY,
            display_EPD_framebuffer());
    }

    // BACK
    cursorX = 20;
    cursorY += 50;
    writeln(
        (GFXfont *)&systemFont,
        "[B] Back",
        &cursorX, &cursorY,
        display_EPD_framebuffer());
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
