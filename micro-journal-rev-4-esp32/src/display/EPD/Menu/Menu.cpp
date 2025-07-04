#include "Menu.h"
#include "app/app.h"
#include "config/config.h"
#include "display/display.h"
#include "../display_EPD.h"

// Menu Sub Modules
#include "Home/Home.h"
#include "Clear/Clear.h"
#include "Wifi/Wifi.h"
#include "Layout/Layout.h"
#include "Sync/Sync.h"
#include "Firmware/Firmware.h"
#include "Reset/Reset.h"

// state
bool menu_clear = false;

// 0 - home
// 1 - sync
// 2 - delete file confirm
// 3 - keyboard layout
int menu_state_prev = -1;

//
void Menu_setup()
{
    // clear screen
    menu_clear = true;

    _log("Entering Menu Setup\n");
}

void Menu_render()
{
    //
    JsonDocument &app = status();

    // check if app clear is set as well
    if (app["clear"].as<bool>() == true)
    {
        menu_clear = true;
    }

    // if menu clear is not set the no need to refresh menu
    if (!menu_clear)
    {
        return;
    }

    // Clear Screen
    epd_poweron();
    epd_clear_quick(epd_full_screen(), 4, 50);
    epd_poweroff_all();

    // deflag so that it doesn't repeatedely called
    menu_clear = false;
    app["clear"] = false;

    ///////////////////////////
    // Render Toolbar
    int cursorX = 10;
    int cursorY = 50;

    String toolbar = " MENU ";
    toolbar += VERSION;
    toolbar += " ";

#ifdef ENV_EPAPER
    toolbar += " REV.7 ";
#endif
    writeln((GFXfont *)&systemFont, toolbar.c_str(), &cursorX, &cursorY, display_EPD_framebuffer());
    epd_draw_hline(0, 58, EPD_WIDTH, 0, display_EPD_framebuffer());

    ///////////////////////////
    // draw sub module of menu
    int menu_state = app["menu"]["state"].as<int>();

    if (menu_state == MENU_HOME)
    {
        if (menu_state_prev != menu_state)
            Home_setup();

        Home_render();
    }

    // Delete File Confirmation Screen
    else if (menu_state == MENU_CLEAR)
    {
        if (menu_state_prev != menu_state)
            Clear_setup();

        Clear_render();
    }

    // Wifi Setup
    else if (menu_state == MENU_WIFI)
    {
        if (menu_state_prev != menu_state)
            Wifi_setup();

        Wifi_render();
    }

    // Keyboard Layout
    else if (menu_state == MENU_LAYOUT)
    {
        if (menu_state_prev != menu_state)
            Layout_setup();

        Layout_render();
    }

    // Sync
    else if (menu_state == MENU_SYNC)
    {
        if (menu_state_prev != menu_state)
            Sync_setup();

        Sync_render();
    }

    // Firmware Update
    else if (menu_state == MENU_FIRMWARE)
    {
        if (menu_state_prev != menu_state)
            Firmware_setup();

        Firmware_render();
    }

    // Reset
    else if (menu_state == MENU_RESET)
    {
        if (menu_state_prev != menu_state)
            Reset_setup();

        Reset_render();
    }

    // render frambuffer
    display_draw_buffer();

    ///////////////////////////
    // save prev state
    menu_state_prev = menu_state;
}

//
void Menu_keyboard(char key)
{
    // every key stroke re-process the menu
    Menu_clear();

    //
    JsonDocument &app = status();

    // clear background for every key stroke
    int menu_state = app["menu"]["state"].as<int>();

    // HOME
    if (menu_state == MENU_HOME)
    {
        Home_keyboard(key);
        return;
    }

    // Delete file
    else if (menu_state == MENU_CLEAR)
    {
        Clear_keyboard(key);
        return;
    }

    // Wifi
    else if (menu_state == MENU_WIFI)
    {
        Wifi_keyboard(key);
        return;
    }

    // Layout
    else if (menu_state == MENU_LAYOUT)
    {
        Layout_keyboard(key);
        return;
    }

    // SYNC
    else if (menu_state == MENU_SYNC)
    {
        Sync_keyboard(key);
        return;
    }

    // Firmware Update
    else if (menu_state == MENU_FIRMWARE)
    {
        Firmware_keyboard(key);
    }

    // Reset
    else if (menu_state == MENU_RESET)
    {
        Reset_keyboard(key);
    }
    
}

//
void Menu_clear()
{
    menu_clear = true;
}
