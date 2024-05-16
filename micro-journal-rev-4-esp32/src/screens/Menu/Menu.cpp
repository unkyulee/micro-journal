#include "Menu.h"
#include "app/app.h"
#include "app/config/config.h"
#include "screens/WordProcessor/WordProcessor.h"
#include "service/display/display.h"

// Menu Sub Modules
#include "Home/Home.h"
#include "Sync/Sync.h"
#include "Clear/Clear.h"
#include "Layout/Layout.h"
#include "Drive/Drive.h"
#include "Wifi/Wifi.h"
#include "BlueToothConfig/BluetoothConfig.h"
#include "FrontPanelButton/FrontPanelButton.h"

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
void Menu_setup(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    //
    JsonDocument &app = app_status();

    // refresh the background
    menu_clear = true;

    // first menu screen
    app["menu"]["state"] = MENU_HOME;
}

void Menu_render(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    // clear background when necessary
    if (menu_clear)
    {
        //
        ptft->fillScreen(TFT_BLACK);

        // toolbr
        ptft->fillRect(0, 0, 320, 20, MENUBAR_COLOR);
        menu_clear = false;
    }

    // display tool bar
    ptft->setCursor(0, 2, 2);
    ptft->setTextColor(TFT_WHITE, MENUBAR_COLOR);
    ptft->setTextSize(1);
    ptft->print(" MENU ");
    ptft->print(VERSION);

    // draw sub module of menu
    JsonDocument &app = app_status();
    int menu_state = app["menu"]["state"].as<int>();

    if (menu_state == MENU_HOME)
    {
        if (menu_state_prev != menu_state)
            Home_setup(ptft, pu8f);

        Home_render(ptft, pu8f);
    }
    else if (menu_state == MENU_SYNC)
    {
        if (menu_state_prev != menu_state)
            Sync_setup(ptft, pu8f);

        Sync_render(ptft, pu8f);
    }
    else if (menu_state == MENU_CLEAR)
    {
        if (menu_state_prev != menu_state)
            Clear_setup(ptft, pu8f);

        Clear_render(ptft, pu8f);
    }
    else if (menu_state == MENU_LAYOUT)
    {
        if (menu_state_prev != menu_state)
            Layout_setup(ptft, pu8f);

        Layout_render(ptft, pu8f);
    }
    else if (menu_state == MENU_WIFI)
    {
        if (menu_state_prev != menu_state)
            Wifi_setup(ptft, pu8f);

        Wifi_render(ptft, pu8f);
    }
    /*
    else if (menu_state == MENU_DRIVE)
    {
        if (menu_state_prev != menu_state)
            Drive_setup(ptft, pu8f);

        Drive_render(ptft, pu8f);
    }
    */
    else if (menu_state == MENU_BLUETOOTH)
    {
        if (menu_state_prev != menu_state)
            BluetoothConfig_setup(ptft, pu8f);

        BluetoothConfig_render(ptft, pu8f);
    }
    else if (menu_state == MENU_BUTTONS)
    {
        if (menu_state_prev != menu_state)
            FrontPanelButton_setup(ptft, pu8f);

        FrontPanelButton_render(ptft, pu8f);
    }

    // save prev state
    menu_state_prev = menu_state;
}

//
void Menu_keyboard(char key)
{
    //
    JsonDocument &app = app_status();

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

    // Keyboard Layout
    else if (menu_state == MENU_LAYOUT)
    {
        Layout_keyboard(key);
        return;
    }

    // Wifi
    else if (menu_state == MENU_WIFI)
    {
        Wifi_keyboard(key);
        return;
    }
    
    /*
    // Drive
    else if (menu_state == MENU_DRIVE)
    {
        Drive_keyboard(key);
        return;
    }
    */

    // Bluetooth Config
    else if (menu_state == MENU_BLUETOOTH)
    {
        BluetoothConfig_keyboard(key);
        return;
    }

    // FrontPanelButton Config
    else if (menu_state == MENU_BUTTONS)
    {
        FrontPanelButton_keyboard(key);
        return;
    }
}

//
void Menu_clear()
{
    menu_clear = true;
}
