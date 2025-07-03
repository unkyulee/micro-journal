#include "Home.h"
#include "../Menu.h"
#include "app/app.h"
#include "display/display.h"

//
int selectedHome = 0;

//
const char *menu[] = {
    "Keyboard",
    "writerDeck",
    "Export File",
    "Info Text",
    "Brightness",
    "Color BG",
    "Color Font",
    "Battery Level",
    "Clear Text",
    "Exit"};

//
void Home_setup(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    //
    Menu_clear();

    selectedHome = 0;
}

//
void Home_render(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    JsonDocument &app = status();

    ptft->setCursor(0, 0);
    ptft->setTextSize(2);

    const int visible_lines = 5;                // Number of menu lines to show
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

    for (int i = 0; i < visible_lines; i++)
    {
        int menu_idx = start + i;
        if (menu_idx >= 0 && menu_idx < menu_length)
        {
            if (menu_idx == selectedHome)
                ptft->setTextColor(TFT_WHITE, TFT_RED);
            else
                ptft->setTextColor(TFT_WHITE, TFT_BLACK);

            ptft->println(menu[menu_idx]);
        }
        else
        {
            ptft->println(""); // Blank line if out of bounds
        }
    }
}

//
void Home_keyboard(char key, bool pressed)
{
    _debug("Home_Keyboard %d selectedHome %d\n", key, selectedHome);
    JsonDocument &app = status();

    // UP
    if (key == 21)
    {
        selectedHome++;
        if (selectedHome >= sizeof(menu) / sizeof(menu[0]))
        {
            selectedHome = 0;
        }
    }

    // DOWN
    else if (key == 20)
    {
        selectedHome--;
        if (selectedHome < 0)
        {
            selectedHome = sizeof(menu) / sizeof(menu[0]) - 1;
        }
    }

    // MENU - SELECTED ACTION
    else if (key == 6 || key == '\n')
    {
        if (!pressed)
        {
            // 0 - USB Keyboard
            if (selectedHome == 0)
            {
                _debug("USB Keyboard Selected\n");
                app["config"]["UsbKeyboard"] = true;
                config_save();

                // load USB Keyboard
                app["screen"] = KEYBOARDSCREEN;
            }

            // 1 - writerDeck
            if (selectedHome == 1)
            {
                _debug("writerDeck Selected\n");
                app["config"]["UsbKeyboard"] = false;
                config_save();

                // load USB Keyboard
                app["screen"] = WORDPROCESSOR;
            }

            // 2 - Export File
            else if (selectedHome == 2)
            {
                _debug("Export File Selected\n");
                app["menu"]["state"] = MENU_STORAGE;
            }

            // 4 - Brightness
            else if (selectedHome == 4)
            {
                _debug("Brightness Selected\n");
                app["menu"]["state"] = MENU_BRIGHTNESS;
            }

            // 5 - Background Color
            else if (selectedHome == 5)
            {
                _debug("Background Color Selected\n");
                app["menu"]["state"] = MENU_BACKGROUND;
            }

            // 6 - Font Color
            else if (selectedHome == 6)
            {
                _debug("Font Color Selected\n");
                app["menu"]["state"] = MENU_FONTCOLOR;
            }

            // last item is EXIT
            else if (selectedHome == sizeof(menu) / sizeof(menu[0]) - 1)
            {
                _debug("Home Keyboard Exit is selected. Moving to Word Processor\n");
                if (app["config"]["UsbKeyboard"].as<bool>())
                {
                    app["screen"] = KEYBOARDSCREEN;
                }
                else
                {
                    app["screen"] = WORDPROCESSOR;
                }
            }
        }
    }
}