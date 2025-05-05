#include "Home.h"
#include "../Menu.h"
#include "app/app.h"
#include "config/config.h"
#include "display/display.h"
#include "editor/editor.h"
#include "keyboard/keyboard.h"
#include <display/EPD/display_EPD.h>
#include "service/Tools/Tools.h"

//
#include <SD.h>
#include <SPIFFS.h>

// Conviences function to get File Size with just an index
size_t FileIndexSize(int index)
{
    char buffer[6];
    sprintf(buffer, "/%d.txt", index);
    size_t file_size = fileSize(buffer);
    return file_size;
}

//
void Home_setup()
{
}

//
void Home_render()
{
    JsonDocument &app = app_status();

    // WIFI
    int cursorX = 10;
    int cursorY = 100;
    writeln((GFXfont *)&systemFont, " [W] WIFI", &cursorX, &cursorY, display_EPD_framebuffer());

    // SYNC
    if (app["config"]["sync"]["url"].as<String>().isEmpty() == false)
    {
        cursorX = 10;
        cursorY += 35;
        writeln((GFXfont *)&systemFont, " [S] SYNC", &cursorX, &cursorY, display_EPD_framebuffer());
    }

    cursorX = 10;
    cursorY += 35;
    writeln((GFXfont *)&systemFont, " [K] Keyboard Layout", &cursorX, &cursorY, display_EPD_framebuffer());

    // Reset
    cursorX = 10;
    cursorY += 35;
    writeln((GFXfont *)&systemFont, " [R] Reset", &cursorX, &cursorY, display_EPD_framebuffer());

    // BACK
    cursorX = 10;
    cursorY += 50;
    writeln((GFXfont *)&systemFont, " [B] BACK", &cursorX, &cursorY, display_EPD_framebuffer());

    // File Selection
    cursorX = 600;
    cursorY = 50;
    writeln((GFXfont *)&systemFont, "CHOOSE A FILE", &cursorX, &cursorY, display_EPD_framebuffer());
    cursorY += 30;

    //
    int file_index = app["config"]["file_index"].as<int>();
    for (int i = 0; i < 10; i++)
    {
        cursorX = 600;
        cursorY += 30;
        writeln(
            (GFXfont *)&systemFont,
            format(" [%d]: %zu", i, FileIndexSize(i)).c_str(),
            &cursorX, &cursorY,
            display_EPD_framebuffer());

        // draw a circle next to the selected file
        if (file_index == i)
        {
            epd_fill_circle(590, cursorY - 8, 8, 0, display_EPD_framebuffer());
        }
    }
    //
    cursorX = 500;
    cursorY += 60;
    writeln(
        (GFXfont *)&systemFont,
        format(" [D] Clear File %d ", file_index).c_str(),
        &cursorX, &cursorY,
        display_EPD_framebuffer());
}

//
void Home_keyboard(char key)
{
    //
    JsonDocument &app = app_status();

    // sync
    if (key == 'S' || key == 's')
    {
        if (app["config"]["sync"]["url"].as<String>().isEmpty() == false)
        {
            // to sync
            app["menu"]["state"] = MENU_SYNC;
        }
    }

    // delete file
    else if (key == 'D' || key == 'd')
    {
        // to clear file
        app["menu"]["state"] = MENU_CLEAR;
    }

    // word processor
    else if (key == '\b' || key == 'B' || key == 'b')
    {
        // go back to the word processor
        app["screen"] = WORDPROCESSOR;
    }

    // wifi setup
    else if (key == 'w')
    {
        // move to keyboard layout
        app["menu"]["state"] = MENU_WIFI;
    }

    // wifi setup
    else if (key == 'k')
    {
        // move to keyboard layout
        app["menu"]["state"] = MENU_LAYOUT;
    }

    // Reset
    else if (key == 'r')
    {
        app["menu"]["state"] = MENU_RESET;
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