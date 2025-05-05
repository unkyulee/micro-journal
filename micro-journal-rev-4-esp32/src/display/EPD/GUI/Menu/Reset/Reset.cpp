#include "Reset.h"
#include "../Menu.h"
#include "app/app.h"
#include "config/config.h"
#include "display/display.h"
#include "editor/editor.h"
#include <display/EPD/display_EPD.h>
#include <SPIFFS.h>

//
void Reset_setup()
{
    // when entering the screen
    // Reset the screen
    Menu_clear();
}

//
void Reset_render()
{
    //
    int cursorX = 10;
    int cursorY = 150;
    writeln(
        (GFXfont *)&systemFont,
        " [Y] ARE YOU SURE TO RESET THE DEVICE?",
        &cursorX, &cursorY,
        display_EPD_framebuffer());

    // Warning
    cursorX = 10;
    cursorY += 50;
    writeln(
        (GFXfont *)&systemFont,
        "WARNING: This action will delete the wifi configuration.",
        &cursorX, &cursorY, display_EPD_framebuffer());

    // BACK
    cursorX = 10;
    cursorY += 150;
    writeln((GFXfont *)&systemFont, " [B] BACK", &cursorX, &cursorY, display_EPD_framebuffer());
}

//
void Reset_keyboard(char key)
{
    JsonDocument &app = app_status();

    // delete confirmed
    if (key == 'Y' || key == 'y')
    {
        // Delete wifi.json from the SPIFF
        SPIFFS.remove("/wifi.json");

        // restart
        ESP.restart();
    }

    // go back to home menu
    //
    app["menu"]["state"] = MENU_HOME;
}
