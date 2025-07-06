#include "Clear.h"
#include "../Menu.h"
#include "app/app.h"
#include "display/display.h"

#include "service/Editor/Editor.h"

//
void Clear_setup(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    //
    Menu_clear();
}

//
void Clear_render(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    ptft->setCursor(0, 0);
    ptft->setTextSize(2);
    ptft->setTextColor(TFT_WHITE, TFT_BLACK);

    ptft->println(""); // Blank line if out of bounds
    ptft->println("Delete Text");
    ptft->setTextSize(1);
    ptft->println("");
    ptft->println("All text will be deleted");
    ptft->println("Press Y to continue");
}

//
void Clear_keyboard(char key, bool pressed)
{
    _debug("Clear_Keyboard %d\n", key);
    JsonDocument &app = status();

    // MENU - SELECTED ACTION
    if (key == 'y')
    {
        // Go back to Home
        _log("Delete File");

        //
        Editor::getInstance().clearFile();

        //
        app["screen"] = WORDPROCESSOR;
    }
    else
    {
        // back to home menu
        app["menu"]["state"] = MENU_HOME;
    }
}