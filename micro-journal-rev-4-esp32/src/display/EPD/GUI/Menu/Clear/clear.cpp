#include "Clear.h"
#include "../Menu.h"
#include "app/app.h"
#include "config/config.h"
#include "display/display.h"
#include "editor/editor.h"
#include <display/EPD/display_EPD.h>

//
void Clear_setup()
{
}

//
void Clear_render()
{
    //
    int cursorX = 10;
    int cursorY = 80;

    // Warning
    cursorX = 10;
    cursorY += 50;
    writeln(
        (GFXfont *)&systemFont,
        "WARNING: This action will delete the selected file.",
        &cursorX, &cursorY,
        display_EPD_framebuffer());

    cursorX = 10;
    cursorY += 50;
    writeln(
        (GFXfont *)&systemFont,
        "Make sure to sync your content before deleting",
        &cursorX, &cursorY,
        display_EPD_framebuffer());

    cursorX = 10;
    cursorY += 80;
    writeln(
        (GFXfont *)&systemFont,
        format(" [Y] ARE YOU SURE TO DELETE %s?", Editor::getInstance().fileBuffer.getFileName()).c_str(),
        &cursorX, &cursorY,
        display_EPD_framebuffer());

    // BACK
    cursorX = 10;
    cursorY += 50;
    writeln((GFXfont *)&systemFont, " [B] BACK", &cursorX, &cursorY, display_EPD_framebuffer());
}

//
void Clear_keyboard(char key)
{
    JsonDocument &app = app_status();

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
