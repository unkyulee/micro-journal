#include "Menu.h"
#include "app/app.h"
#include "display/display.h"
#include "../WordProcessor/WordProcessor.h"

// Menu Sub Modules
#include "Home/Home.h"
#include "Sync/Sync.h"
#include "Clear/Clear.h"
#include "Wifi/Wifi.h"
#include "Background/Background.h"
#include "Foreground/Foreground.h"
#include "Brightness/Brightness.h"

//
#include "display/CARDPUTER/display_CARDPUTER.h"

// properties
#define MENUBAR_COLOR TFT_RED

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
    //
    JsonDocument &app = status();

    // refresh the background
    menu_clear = true;
    menu_state_prev = -1;
    app["menu"]["state"] = MENU_HOME;
}

void Menu_render()
{
    JsonDocument &app = status();

    // check if app clear is set as well
    if (app["clear"].as<bool>() == true)
    {
        // reset global clear flag
        app["clear"] = false;

        // clear screen
        menu_clear = true;
    }

    if (!menu_clear)
    {
        // nothing is changed
        // skip this rendering cycle
        return;
    }

    // clear screen
    {
        M5Cardputer.Display.fillRect(
            0,
            0,
            M5Cardputer.Display.width(),
            M5Cardputer.Display.height(),
            TFT_BLACK);

        //
        menu_clear = false;
    }

    // draw sub module of menu
    int menu_state = app["menu"]["state"].as<int>();

    if (menu_state == MENU_HOME)
    {
        if (menu_state_prev != menu_state)
            Home_setup();

        Home_render();
    }

    else if (menu_state == MENU_SYNC)
    {
        if (menu_state_prev != menu_state)
            Sync_setup();

        Sync_render();
    }

    else if (menu_state == MENU_WIFI)
    {
        if (menu_state_prev != menu_state)
            Wifi_setup();

        Wifi_render();
    }

    else if (menu_state == MENU_BRIGHTNESS)
    {
        if (menu_state_prev != menu_state)
            Brightness_setup();

        Brightness_render();
    }

    else if (menu_state == MENU_BACKGROUND)
    {
        if (menu_state_prev != menu_state)
            Background_setup();

        Background_render();
    }

    else if (menu_state == MENU_FONTCOLOR)
    {
        if (menu_state_prev != menu_state)
            Foreground_setup();

        Foreground_render();
    }

    else if (menu_state == MENU_CLEAR)
    {
        if (menu_state_prev != menu_state)
            Clear_setup();

        Clear_render();
    }

    // save prev state
    menu_state_prev = menu_state;
}

//
void Menu_keyboard(char key)
{
    //
    JsonDocument &app = status();

    // clear background for every key stroke
    Menu_clear();

    // based on the current menu state
    int menu_state = app["menu"]["state"].as<int>();
    if (menu_state == MENU_HOME)
    {
        Home_keyboard(key);
        return;
    }

    // SYNC MENU
    else if (menu_state == MENU_SYNC)
    {
        Sync_keyboard(key);
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

    //
    else if (menu_state == MENU_BACKGROUND)
    {
        Background_keyboard(key);
    }

    //
    else if (menu_state == MENU_FONTCOLOR)
    {
        Foreground_keyboard(key);
    }

    //
    else if (menu_state == MENU_BRIGHTNESS)
    {
        Brightness_keyboard(key);
    }
}

//
void Menu_clear()
{
    menu_clear = true;
}
