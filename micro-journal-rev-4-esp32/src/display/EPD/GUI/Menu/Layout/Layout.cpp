#include "Layout.h"
#include "../Menu.h"
#include "app/app.h"
#include "config/config.h"
#include "display/display.h"
#include "editor/editor.h"
#include "keyboard/keyboard.h"
#include <display/EPD/display_EPD.h>

//
void Layout_setup()
{
    // when entering the screen
    // clear the screen
    Menu_clear();
}

//
void Layout_render()
{
    JsonDocument &app = app_status();

    // Choose a keyboard layout
    int cursorX = 10;
    int cursorY = 100;
    writeln((GFXfont *)&systemFont, " - Choose a Keyboard Layout", &cursorX, &cursorY, display_EPD_framebuffer());
    cursorY += 20;

    cursorX = 10;
    cursorY += 40;
    writeln((GFXfont *)&systemFont, "[a] International", &cursorX, &cursorY, display_EPD_framebuffer());
    
    cursorX = 10;
    cursorY += 40;
    writeln((GFXfont *)&systemFont, "[b] Belgian", &cursorX, &cursorY, display_EPD_framebuffer());
    
    cursorX = 10;
    cursorY += 40;
    writeln((GFXfont *)&systemFont, "[c] Canadian Multiligual", &cursorX, &cursorY, display_EPD_framebuffer());
    
    cursorX = 10;
    cursorY += 40;
    writeln((GFXfont *)&systemFont, "[g] German", &cursorX, &cursorY, display_EPD_framebuffer());
    
    cursorX = 10;
    cursorY += 40;
    writeln((GFXfont *)&systemFont, "[i] Italian", &cursorX, &cursorY, display_EPD_framebuffer());
    
    cursorX = 10;
    cursorY += 40;
    writeln((GFXfont *)&systemFont, "[k] UK", &cursorX, &cursorY, display_EPD_framebuffer());
    
    cursorX = 10;
    cursorY += 40;
    writeln((GFXfont *)&systemFont, "[u] US", &cursorX, &cursorY, display_EPD_framebuffer());

    // BACK
    cursorX = 10;
    cursorY += 50;
    writeln((GFXfont *)&systemFont, " [B] BACK", &cursorX, &cursorY, display_EPD_framebuffer());

}

//
void Layout_keyboard(char key)
{
    //
    JsonDocument &app = app_status();

    if (key == 'b')
    {
        // canadian multiligual
        app["config"]["keyboard_layout"] = "BE";
        config_save();
        // go back to the word processor
        app["screen"] = WORDPROCESSOR;
    }

    else if (key == 'c')
    {
        // canadian multiligual
        app["config"]["keyboard_layout"] = "CA";
        config_save();
        // go back to the word processor
        app["screen"] = WORDPROCESSOR;
    }

    else if (key == 'g')
    {
        // italian
        app["config"]["keyboard_layout"] = "GE";
        config_save();
        // go back to the word processor
        app["screen"] = WORDPROCESSOR;
    }

    else if (key == 'i')
    {
        // italian
        app["config"]["keyboard_layout"] = "IT";
        config_save();
        // go back to the word processor
        app["screen"] = WORDPROCESSOR;
    }

    else if (key == 'k')
    {
        // italian
        app["config"]["keyboard_layout"] = "UK";
        config_save();
        // go back to the word processor
        app["screen"] = WORDPROCESSOR;
    }

    else if (key == 'u')
    {
        // us
        app["config"]["keyboard_layout"] = "US";
        config_save();
        // go back to the word processor
        app["screen"] = WORDPROCESSOR;
    }

    else if (key == 'a')
    {
        // us
        app["config"]["keyboard_layout"] = "INT";
        config_save();
        // go back to the word processor
        app["screen"] = WORDPROCESSOR;
    }

    //
    // go back to home menu
    app["menu"]["state"] = MENU_HOME;
}
