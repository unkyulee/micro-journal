#include "Home.h"
#include "../Menu.h"
#include "app/app.h"
#include "display/display.h"

//
int selectedHome = 0;

//
const char *menu[] = {
    "Keyboard",    // 0
    "writerDeck",  // 1
    "Drive Mode",  // 2
    "Send Text",   // 3
    "Info Text",   // 4
    "Brightness",  // 5
    "Color BG",    // 6
    "Color Font",  // 7
    "FW Update",   // 8
    "Delete Text", // 9
    "Exit"};

//
void Home_setup(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    //
    Menu_clear();

    // if UsbKeyboard is selected or not
    JsonDocument &app = status();
    bool UsbKeyboard = app["config"]["UsbKeyboard"].as<bool>();
    if (UsbKeyboard)
        selectedHome = 0;
    else
        selectedHome = 1; // if not choose writerDeck
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
    //
    _log("Home_Keyboard %d selectedHome %d\n", key, selectedHome);
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
    else if (key == 27 || key == '\n' || key == MENU)
    {
        // when ESC is clicked escape
        if (key == 27)
            selectedHome = sizeof(menu) / sizeof(menu[0]) - 1;

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

        // 2 - Drive Mode
        else if (selectedHome == 2)
        {
            _debug("Export File Selected\n");
            app["menu"]["state"] = MENU_STORAGE;
        }

        // 3 - Send Text
        else if (selectedHome == 3)
        {
            _debug("Send Text Selected\n");
            app["menu"]["state"] = MENU_SYNC;
        }

        // 4 - Info Text
        else if (selectedHome == 4)
        {
            _debug("Info Text Selected\n");
            app["menu"]["state"] = MENU_INFO;
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

        // 8 - Firmware Update
        else if (selectedHome == 8)
        {
            _debug("Firmware Update Selected\n");

            // Reboot RP2040 with Boot Mode
            rp2040.rebootToBootloader();
        }

        // 9 - Clear Text
        else if (selectedHome == 9)
        {
            _debug("Clear Text Selected\n");

            // Delete file content
            app["menu"]["state"] = MENU_CLEAR;
        }

        // last item is EXIT
        else if (selectedHome == sizeof(menu) / sizeof(menu[0]) - 1)
        {
            bool UsbKeyboard = app["config"]["UsbKeyboard"].as<bool>();
            _log("Exit is selected. UsbKeyboard: %d\n", UsbKeyboard);
            if (UsbKeyboard)
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