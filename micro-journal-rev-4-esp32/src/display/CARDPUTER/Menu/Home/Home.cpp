#include "Home.h"
#include "../Menu.h"
#include "app/app.h"
#include "display/display.h"
#include "keyboard/keyboard.h"

//
#include "service/Editor/Editor.h"

//
#include "display/CARDPUTER/display_CARDPUTER.h"

//
int selectedHome = 0;

//
const char *menu[] = {
    "Sync",         // 0
    "WIFI",         // 1
    "BLE KEYBOARD", // 2
    "BLE HOST",     // 3
    "Language",     // 4
    "Brightness",   // 5
    "Color BG",     // 6
    "Color Font",   // 7
    "Delete Text",  // 8
    "Exit"};

//
void Home_setup()
{
    //
    Menu_clear();
}

//
void Home_render()
{
    JsonDocument &app = status();

    M5Cardputer.Display.setTextSize(0.7);
    M5Cardputer.Display.setFont(&fonts::Orbitron_Light_24);

    const int visible_lines = 6;                // Number of menu lines to show
    const int center_index = visible_lines / 2; // 2 for 5 lines (3rd line)
    int menu_length = sizeof(menu) / sizeof(menu[0]);

    // Calculate the start index so that selectedHome is always at center_index
    int start = selectedHome - center_index;
    if (start < 0)
        start = 0;
    if (start > menu_length - visible_lines)
        start = menu_length - visible_lines;
    if (start < 0)
        start = 0; // In case menu_length < visible_lines

    int y = 0;
    for (int i = 0; i < visible_lines; i++)
    {
        int menu_idx = start + i;
        if (menu_idx >= 0 && menu_idx < menu_length)
        {
            if (menu_idx == selectedHome)
                M5Cardputer.Display.setTextColor(TFT_WHITE, TFT_RED);
            else
                M5Cardputer.Display.setTextColor(TFT_WHITE, TFT_BLACK);

            M5Cardputer.Display.drawString(menu[menu_idx], 5, y);
            y += 22;
        }
        else
        {
            y += 22;
        }
    }
}

//
void Home_keyboard(char key)
{
    //
    JsonDocument &app = status();

    // DOWN
    if (key == 21 || key == '.')
    {
        selectedHome++;
        if (selectedHome >= sizeof(menu) / sizeof(menu[0]))
        {
            selectedHome = 0;
        }
    }

    // UP
    else if (key == 20 || key == ';')
    {
        selectedHome--;
        if (selectedHome < 0)
        {
            selectedHome = sizeof(menu) / sizeof(menu[0]) - 1;
        }
    }

    // MENU - SELECTED ACTION
    else if (key == 27 || key == '\n' || key == MENU)
    {
        // when ESC is clicked escape
        if (selectedHome == sizeof(menu) / sizeof(menu[0]) - 1 || key == 27)
        {
            app["screen"] = WORDPROCESSOR;
        }

        // 0 - Sync
        else if (selectedHome == 0)
        {
            _debug("Sync Selected\n");
            app["menu"]["state"] = MENU_SYNC;
        }

        // 1 - WIFI
        else if (selectedHome == 1)
        {
            _debug("WIFI Selected\n");
            app["menu"]["state"] = MENU_WIFI;
        }

        // 2 - BLE Keyboard
        else if (selectedHome == 2)
        {
            _debug("BLE Keyboard Selected\n");
            app["screen"] = KEYBOARDSCREEN;
        }

        // 3 - BLE Host
        else if (selectedHome == 3)
        {
            _debug("BLE Host Selected\n");
            app["menu"]["state"] = MENU_BLUETOOTH;
        }

        // 4 - Brightness
        else if (selectedHome == 4)
        {
            _debug("Language Selected\n");
            app["menu"]["state"] = MENU_LANGUAGE;
        }

        // 5 - Brightness
        else if (selectedHome == 5)
        {
            _debug("Brightness Selected\n");
            app["menu"]["state"] = MENU_BRIGHTNESS;
        }

        // 6 - Background Color
        else if (selectedHome == 6)
        {
            _debug("Background Color Selected\n");
            app["menu"]["state"] = MENU_BACKGROUND;
        }

        // 7 - Font Color
        else if (selectedHome == 7)
        {
            _debug("Font Color Selected\n");
            app["menu"]["state"] = MENU_FONTCOLOR;
        }

        // 8 - Clear Text
        else if (selectedHome == 8)
        {
            _debug("Clear Text Selected\n");

            // Delete file content
            app["menu"]["state"] = MENU_CLEAR;
        }
    }
}
