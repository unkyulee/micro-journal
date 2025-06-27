#include "Sleep.h"
#include "../../display_EPD.h"
#include "editor/editor.h"

//
#include "app/app.h"
#include "display/display.h"
#include "keyboard/keyboard.h"

bool sleep_rendered = false;
//
void Sleep_setup()
{
    //
    _log("Sleep Screen Setup\n");

    // render just once
    sleep_rendered = false;
}

//
void Sleep_render()
{
    //
    if (sleep_rendered)
        return;

    //
    sleep_rendered = true;

    // SAVE FILE
    Editor::getInstance().saveFile();

    // Turn on the display
    epd_poweron();

    // Clear Screen
    epd_clear();

    // Render Image
    int32_t x = 20;
    int32_t y = 50;
    write_string(display_EPD_font(), "Please, turn off the device", &x, &y, NULL);
    
    // Go to Sleep mode
    epd_poweroff_all();
    esp_deep_sleep_start();
}

//
void Sleep_keyboard(char key)
{
    // Any Keystroke moves to word processor
    JsonDocument &app = status();
    app["screen"] = WORDPROCESSOR;
}
