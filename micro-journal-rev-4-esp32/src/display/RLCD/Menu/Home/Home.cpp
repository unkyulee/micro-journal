#include "Home.h"
#include "../Menu.h"
#include "app/app.h"
#include "display/display.h"
#include "keyboard/keyboard.h"

//
#include "service/Editor/Editor.h"

//
void Home_setup(ST7305_4p2_BW_DisplayDriver *display, U8G2_FOR_ST73XX *u8)
{
    //
    Menu_clear();

    //
    JsonDocument &app = status();
    app["menu"]["state"] = MENU_HOME;
}

//
void Home_render(ST7305_4p2_BW_DisplayDriver *display, U8G2_FOR_ST73XX *u8)
{
    JsonDocument &app = status();

    //
    String keyboard_layout = app["config"]["keyboard_layout"].as<String>();
    if (keyboard_layout == "null" || keyboard_layout.isEmpty())
        keyboard_layout = "US";

    // Text to be displayed
    u8->setCursor(0, 40);
    u8->println();
    //

    //
    if (app["config"]["sync"]["url"].as<String>().isEmpty() == false)
    {
        u8->println(" [S] SYNC ");
    }

    u8->println(" [W] WIFI");
    u8->println(" [L] LANGUAGE");
    u8->println(" [T] BLE KEYBOARD");
    u8->println(" [U] DRIVE MODE");

    u8->println();
    u8->println(" [B] BACK ");

    // File Selection
    int pos_x = 190;
    u8->setCursor(pos_x, 45);
    u8->print("CHOOSE A FILE");

    int file_index = app["config"]["file_index"].as<int>();
    for (int i = 0; i < 10; i++)
    {
        // word count
        int wordCount = app["config"][format("wordcount_file_%d", i)].as<int>() + app["config"][format("wordcount_buffer_%d", i)].as<int>();

        u8->setCursor(pos_x, 75 + i * 20);
        u8->printf(" [%d]: %zu", i, wordCount);
    }
    //
    u8->setCursor(pos_x, 290);
    u8->printf(" [D] Clear File # %d", file_index);
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

    else if (key == 'w')
    {
        // move to keyboard layout
        app["menu"]["state"] = MENU_WIFI;
    }

    else if (key == 'l')
    {
        // move to keyboard layout
        app["menu"]["state"] = MENU_LAYOUT;
    }

    else if (key == 't')
    {
        // move to keyboard layout
        app["screen"] = KEYBOARDSCREEN;
    }

    else if (key == 'u')
    {
        // move to keyboard layout
        app["menu"]["state"] = MENU_STORAGE;
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