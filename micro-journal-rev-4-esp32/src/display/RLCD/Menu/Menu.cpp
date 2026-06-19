#include "Menu.h"
#include "app/app.h"
#include "display/display.h"
#include "../WordProcessor/WordProcessor.h"

// Menu Sub Modules
#include "Home/Home.h"
#include "Sync/Sync.h"
#include "Clear/Clear.h"
#include "Layout/Layout.h"
#include "Wifi/Wifi.h"
#include "Storage/Storage.h"

// state
bool menu_clear = false;

// Whether the last Menu_render() actually changed anything visible - the
// caller uses this to decide whether the (expensive, full 30KB SPI) panel
// refresh is worth doing this tick.
static bool needsDisplay = true;

// 0 - home
// 1 - sync
// 2 - delete file confirm
// 3 - keyboard layout
int menu_state_prev = -1;

//
void Menu_setup(ST7305_4p2_BW_DisplayDriver *display, U8G2_FOR_ST73XX *u8)
{
    //
    JsonDocument &app = status();

    // refresh the background
    menu_clear = true;

    //
    app["menu"]["state"] = MENU_HOME;

    //
    
}

void Menu_render(ST7305_4p2_BW_DisplayDriver *display, U8G2_FOR_ST73XX *u8)
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
        needsDisplay = false;
        return;
    }

    // decide whether the panel actually needs to be pushed over SPI this tick
    needsDisplay = true;

    // clear screen
    {
        //
        display->clearDisplay();

        //
        menu_clear = false;
    }

    // display tool bar
    u8->setFont(u8g2_font_profont17_tf);
    u8->setCursor(0, 5);
    u8->println();
    u8->print(" MENU ");
    u8->print(VERSION);
    display->drawLine(0, 25, 400, 25, 1);

    // draw sub module of menu
    int menu_state = app["menu"]["state"].as<int>();

    if (menu_state == MENU_HOME)
    {
        if (menu_state_prev != menu_state)
            Home_setup(display, u8);

        Home_render(display, u8);
    }
    else if (menu_state == MENU_LAYOUT)
    {
        if (menu_state_prev != menu_state)
            Layout_setup(display, u8);

        Layout_render(display, u8);
    }
    else if (menu_state == MENU_SYNC)
    {
        if (menu_state_prev != menu_state)
            Sync_setup(display, u8);

        Sync_render(display, u8);
    }
    else if (menu_state == MENU_CLEAR)
    {
        if (menu_state_prev != menu_state)
            Clear_setup(display, u8);

        Clear_render(display, u8);
    }
    else if (menu_state == MENU_WIFI)
    {
        if (menu_state_prev != menu_state)
            Wifi_setup(display, u8);

        Wifi_render(display, u8);
    }
    else if (menu_state == MENU_STORAGE)
    {
        if (menu_state_prev != menu_state)
            Storage_setup(display, u8);

        Storage_render(display, u8);
    }

    // save prev state
    menu_state_prev = menu_state;
}

bool Menu_needsDisplay()
{
    return needsDisplay;
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

    // LAYOUT MENU
    else if (menu_state == MENU_LAYOUT)
    {
        Layout_keyboard(key);
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

    // Storage
    else if (menu_state == MENU_STORAGE)
    {
        Storage_keyboard(key);
        return;
    }
}

//
void Menu_clear()
{
    menu_clear = true;
}
