#include "WordProcessor.h"
#include "../../display_EPD.h"

//
#include "app/app.h"
#include "editor/editor.h"
#include "keyboard/keyboard.h"
#include "display/display.h"
#include "keyboard/hid/nimble/ble.h"

//
void WP_setup()
{
    app_log("Word Process GUI Setup\n");

    // Turn on the display
    epd_poweron();

    // Clear Screen
    epd_clear();

    // Turn off the display
    epd_poweroff();

    // editor instantiate
    Editor::getInstance();

    // load file from the editor
    JsonDocument &app = app_status();
    int file_index = app["config"]["file_index"].as<int>();
    
    //
    //Editor::getInstance().loadFile(format("/%d.txt", file_index));

    //
    app_log("Word Processor Initialized %d.txt\n", file_index);
}

//
void WP_render()
{
    // Turn on the display
    epd_poweron();

    // Clear Screen
    //epd_clear();

    // RENDER TEXT
    WP_render_text();

    // Turn off the display
    epd_poweroff();
}

//
void WP_render_text()
{
    int line_num = 0;

    char *line = Editor::getInstance().screenBuffer.line_position[line_num];
    int length = Editor::getInstance().screenBuffer.line_length[line_num];

    int32_t x = 18;
    int32_t y = 50;
    writeln((GFXfont *)&FiraSans, "Ready", &x, &y, NULL);
    y += 50;
    x = 18;
    
    if(line != NULL)
        writeln((GFXfont *)&FiraSans, line, &x, &y, NULL);
}

//
void WP_keyboard(char key)
{
    JsonDocument &app = app_status();

    // Check if menu key is pressed
    if (key == MENU)
    {
        // Save before transitioning to the menu
        Editor::getInstance().saveFile();

        //
        app["screen"] = MENUSCREEN;
    }

    else
    {
        // send the keys to the editor
        Editor::getInstance().keyboard(key);
    }
}
