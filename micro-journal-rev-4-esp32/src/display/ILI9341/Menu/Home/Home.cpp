#include "Home.h"
#include "../Menu.h"
#include "app/app.h"
#include "display/display.h"
#include "keyboard/keyboard.h"

//
#include "service/Editor/Editor.h"

//
void Home_setup(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    //
    Menu_clear();

    //
    JsonDocument &app = status();
    app["menu"]["state"] = MENU_HOME;
}

//
void Home_render(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    JsonDocument &app = status();

    //
    String keyboard_layout = app["config"]["keyboard_layout"].as<String>();
    if (keyboard_layout == "null" || keyboard_layout.isEmpty())
        keyboard_layout = "US";

    // Text to be displayed
    ptft->setCursor(0, 20, 2);
    ptft->setTextSize(1);

    //
    ptft->setTextColor(TFT_WHITE, TFT_BLACK);
    ptft->println();

    //
    if (app["config"]["sync"]["url"].as<String>().isEmpty() == false)
    {
        ptft->println(" [S] SYNC ");
    }

#ifdef REV5
    ptft->println(" [K] KEY LAYOUT - " + keyboard_layout);
    ptft->println(" [F] DEVICE BUTTON");
    ptft->println(" [M, MENU] BLE KEYBOARD");
#endif
    ptft->println(" [W] WIFI");

#ifdef REV6
    ptft->println(" [T] BLE KEYBOARD");
#endif

    ptft->println(" [G] BACKGROUND COLOR");
    ptft->println(" [C] FONT COLOR");

    ptft->println();
    ptft->println(" [B] BACK ");
    ptft->println();
    ptft->println();

    // File Selection
    int pos_x = 190;
    ptft->setCursor(pos_x, 25, 2);
    ptft->print("CHOOSE A FILE");

    int file_index = app["config"]["file_index"].as<int>();
    for (int i = 0; i < 10; i++)
    {
        if (file_index == i)
        {
            ptft->setTextColor(TFT_GREEN, TFT_BLACK);
        }
        else
        {
            ptft->setTextColor(TFT_WHITE, TFT_BLACK);
        }

        // word count
        int wordCount = app["config"][format("wordcount_file_%d", i)].as<int>() + app["config"][format("wordcount_buffer_%d", i)].as<int>();

        ptft->setCursor(pos_x, 45 + i * 16, 2);
        ptft->printf(" [%d]: %zu", i, wordCount);
    }
    //
    ptft->setCursor(pos_x, 210, 2);
    ptft->printf(" [D] Clear File [%d] ", file_index);
}

//
void Home_keyboard(char key)
{
    //
    JsonDocument &app = status();

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

#ifdef REV5
    else if (key == 'k')
    {
        // move to keyboard layout
        app["menu"]["state"] = MENU_LAYOUT;
    }
    else if (key == 'f')
    {
        // move to keyboard layout
        app["menu"]["state"] = MENU_BUTTONS;
    }
    else if (key == 'm' || key == MENU)
    {
        // move to bluetooth setup
        app["menu"]["state"] = MENU_BLUETOOTH;
    }

#endif

    else if (key == 'w')
    {
        // move to keyboard layout
        app["menu"]["state"] = MENU_WIFI;
    }

#ifdef REV6
    else if (key == 't')
    {
        // move to keyboard layout
        app["screen"] = KEYBOARDSCREEN;
    }
#endif

    else if (key == 'g')
    {
        // move to keyboard layout
        app["menu"]["state"] = MENU_BACKGROUND;
    }

    else if (key == 'c')
    {
        // move to keyboard layout
        app["menu"]["state"] = MENU_FONTCOLOR;
    }

    // chose file
    if (key > 47 && key < 58)
    {
        // save config
        int file_index = key - 48;
        app["config"]["file_index"] = file_index;
        config_save();

        // load editor
        Editor::getInstance().loadFile(format("/%d.txt", file_index));

        //
        // go back to the word processor
        app["screen"] = WORDPROCESSOR;
    }
}