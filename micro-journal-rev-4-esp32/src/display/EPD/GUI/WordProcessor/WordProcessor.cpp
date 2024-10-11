#include "WordProcessor.h"
#include "../../display_EPD.h"

//
#include "app/app.h"
#include "editor/editor.h"
#include "keyboard/keyboard.h"
#include "display/display.h"
#include "keyboard/hid/nimble/ble.h"

//
#include "firasans.h"

//
void WP_setup()
{
    // editor instantiate
    Editor::getInstance();

    // load file from the editor
    JsonDocument &app = app_status();
    int file_index = app["config"]["file_index"].as<int>();
    Editor::getInstance().loadFile(format("/%d.txt", file_index));

    //
    app_log("Word Processor Initialized %d.txt\n", file_index);
}

//
void WP_render()
{
    // Turn on the display
    epd_poweron();

    // Clear Screen
    // epd_clear();

    // RENDER TEXT
    WP_render_text();

    // Turn off the display
    epd_poweroff();
}

//
void WP_render_text()
{
    int32_t x = 18;
    int32_t y = 50;
    writeln((GFXfont *)&FiraSans, "➸ Touch is online  😀 \n", &x, &y, NULL);

    JsonDocument &app = app_status();
    if(app["hid_usb"].as<bool>() == true) {
        writeln((GFXfont *)&FiraSans, "USB Connected \n", &x, &y, NULL);
    } else {
        writeln((GFXfont *)&FiraSans, "USB Not Connected \n", &x, &y, NULL);
    }
}

//
void WP_keyboard(char key)
{
    // Check if menu key is pressed
    if (key == MENU)
    {
        // Save before transitioning to the menu
        Editor::getInstance().saveFile();

        //
        JsonDocument &app = app_status();
        app["screen"] = MENUSCREEN;
    }

    else
    {
        // send the keys to the editor
        Editor::getInstance().keyboard(key);
    }
}
