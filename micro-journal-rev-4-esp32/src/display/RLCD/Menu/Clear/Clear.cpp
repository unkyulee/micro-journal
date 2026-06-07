#include "Clear.h"
#include "../Menu.h"
#include "app/app.h"
#include "display/display.h"
#include "service/Editor/Editor.h"

//
void Clear_setup(ST7305_4p2_BW_DisplayDriver *display, U8G2_FOR_ST73XX *u8)
{
    // when entering the screen
    // clear the screen
    Menu_clear();
}

//
void Clear_render(ST7305_4p2_BW_DisplayDriver *display, U8G2_FOR_ST73XX *u8)
{    
    //
    u8->setCursor(0, 50);
    u8->println();
    u8->println(" WARNING: This action will delete");
    u8->println(" the selected file.");
    u8->println(" Make sure to sync your content");
    u8->println(" before confirming");
    u8->println(" to prevent loss of data.");
    u8->println();
    u8->println();
    u8->println(" [Y] ARE YOU SURE?");
    u8->println(" [B] BACK ");
}

//
void Clear_keyboard(char key)
{
    JsonDocument &app = status();

    // delete confirmed
    if (key == 'Y' || key == 'y')
    {
        // empty the file
        Editor::getInstance().clearFile();

        // go back to the word processor
        app["screen"] = WORDPROCESSOR;
    }

    // go back to home menu
    //
    app["menu"]["state"] = MENU_HOME;
}
