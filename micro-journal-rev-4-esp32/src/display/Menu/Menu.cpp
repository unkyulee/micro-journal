#include "Menu.h"
#include "app/app.h"
#include "config/config.h"
#include "display/display.h"
#include "display/WordProcessor/WordProcessor.h"

// Menu Sub Modules
#include "Home/Home.h"
#include "Sync/Sync.h"
#include "Clear/Clear.h"
#include "Layout/Layout.h"
#include "Wifi/Wifi.h"
#include "FrontPanelButton/FrontPanelButton.h"
#include "Background/Background.h"
#include "Foreground/Foreground.h"
#include "DisableWakeUp/DisableWakeUp.h"
#include "Reset/Reset.h"
#include "Firmware/Firmware.h"

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
    ptft->print(" ");

#ifdef ENV_USBHOST
    ptft->print(" REV.5 ");
#endif

#ifdef ENV_KEYBOARD
    ptft->print(" REV.6 ");
#endif

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
    else if (menu_state == MENU_WIFI)
    {
        if (menu_state_prev != menu_state)
            Wifi_setup(ptft, pu8f);

        Wifi_render(ptft, pu8f);
    }
    //
    else if (menu_state == MENU_STARTUP)
    {
        if (menu_state_prev != menu_state)
            DisableWakeUp_setup(ptft, pu8f);

        DisableWakeUp_render(ptft, pu8f);
    }
    else if (menu_state == MENU_FIRMWARE)
    {
        if (menu_state_prev != menu_state)
            Firmware_setup(ptft, pu8f);

        Firmware_render(ptft, pu8f);
    }
    else if (menu_state == MENU_RESET)
    {
        if (menu_state_prev != menu_state)
            Reset_setup(ptft, pu8f);

        Reset_render(ptft, pu8f);
    }
    else if (menu_state == MENU_BACKGROUND)
    {
        if (menu_state_prev != menu_state)
            Background_setup(ptft, pu8f);

        Background_render(ptft, pu8f);
    }
    else if (menu_state == MENU_FOREGROUND)
    {
        if (menu_state_prev != menu_state)
            Foreground_setup(ptft, pu8f);

        Foreground_render(ptft, pu8f);
    }

#ifdef ENV_USBHOST
    else if (menu_state == MENU_LAYOUT)
    {
        if (menu_state_prev != menu_state)
            Layout_setup(ptft, pu8f);

        Layout_render(ptft, pu8f);
    }
    else if (menu_state == MENU_BUTTONS)
    {
        if (menu_state_prev != menu_state)
            FrontPanelButton_setup(ptft, pu8f);

        FrontPanelButton_render(ptft, pu8f);
    }
#endif

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

    // Wifi
    else if (menu_state == MENU_WIFI)
    {
        Wifi_keyboard(key);
        return;
    }

    //
    else if (menu_state == MENU_STARTUP)
    {
        DisableWakeUp_keyboard(key);
        return;
    }

    //
    else if (menu_state == MENU_BACKGROUND)
    {
        Background_keyboard(key);
    }

    //
    else if (menu_state == MENU_FOREGROUND)
    {
        Foreground_keyboard(key);
    }

    //
    else if (menu_state == MENU_RESET)
    {
        Reset_keyboard(key);
    }

    //
    else if (menu_state == MENU_FIRMWARE)
    {
        Firmware_keyboard(key);
    }

#ifdef ENV_USBHOST
    // Keyboard Layout
    else if (menu_state == MENU_LAYOUT)
    {
        Layout_keyboard(key);
        return;
    }
    // FrontPanelButton Config
    else if (menu_state == MENU_BUTTONS)
    {
        FrontPanelButton_keyboard(key);
        return;
    }
#endif
}

//
void Menu_clear()
{
    menu_clear = true;
}
