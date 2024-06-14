#include "Home.h"
#include "../Menu.h"
#include "app/app.h"
#include "config/config.h"
#include "display/display.h"
#include "display/WordProcessor/WordProcessor.h"

//
void Home_setup(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    //
    Menu_clear();
}

//
void Home_render(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    JsonDocument &app = app_status();

    //
    String keyboard_layout = app["config"]["keyboard_layout"].as<String>();
    if (keyboard_layout == "null" || keyboard_layout.isEmpty())
        keyboard_layout = "US";

    // Text to be displayed
    ptft->setCursor(0, 30, 2);
    ptft->setTextSize(1);

    //
    ptft->setTextColor(TFT_WHITE, TFT_BLACK);
    ptft->println();

    //
    if (app["config"]["sync"]["url"].as<String>().isEmpty() == false)
    {
        ptft->println(" [S] SYNC ");
    }

    //
    ptft->println(" [D] START NEW ");
#ifdef ENV_USBHOST
    ptft->println(" [K] KEYBOARD LAYOUT - " + keyboard_layout);
#endif
    ptft->println(" [W] WIFI");
    ptft->println(" [G] BACKGROUND COLOR");
    ptft->println(" [C] FOREGROUND COLOR");
    ptft->println(" [F] DEVICE BUTTON");
    ptft->println(" [R] REBOOT DEVICE");
    ptft->println();
    ptft->println(" [B] BACK ");
    ptft->println();
    ptft->println();
}

//
void Home_keyboard(char key)
{
    //
    JsonDocument &app = app_status();

    // home
    if (key == 'S' || key == 's')
    {
        if (app["config"]["sync"]["url"].as<String>().isEmpty() == false)
        {
            // to sync
            app["menu"]["state"] = MENU_SYNC;
        }
    }

    else if (key == 'D' || key == 'd')
    {
        // to clear file
        app["menu"]["state"] = MENU_CLEAR;
    }

    else if (key == '\b' || key == 'B' || key == 'b')
    {
        // go back to the word processor
        app["screen"] = WORDPROCESSOR;
    }

#ifdef ENV_USBHOST
    else if (key == 'k')
    {
        // move to keyboard layout
        app["menu"]["state"] = MENU_LAYOUT;
    }
#endif

    else if (key == 'w')
    {
        // move to keyboard layout
        app["menu"]["state"] = MENU_WIFI;
    }

    else if (key == 'g')
    {
        // move to keyboard layout
        app["menu"]["state"] = MENU_BACKGROUND;
    }

    else if (key == 'c')
    {
        // move to keyboard layout
        app["menu"]["state"] = MENU_FOREGROUND;
    }

    else if (key == 'r')
    {
        // restart
        ESP.restart();
    }

    else if (key == 'f')
    {
        // move to keyboard layout
        app["menu"]["state"] = MENU_BUTTONS;
    }
}