#include "Update.h"
#include "app/app.h"
#include "display/display.h"
#include <display/EPD/display_EPD.h>

//
#include "service/Updater/Updater.h"

//
void Update_setup()
{
    // Clear Screen
    epd_poweron();
    epd_clear_quick(epd_full_screen(), 4, 50);
    epd_poweroff_all();

    //
    int cursorX = 10;
    int cursorY = 150;
    writeln(
        (GFXfont *)&systemFont,
        "Press any key to proceed with the update",
        &cursorX, &cursorY,
        display_EPD_framebuffer());

    // render frambuffer
    display_draw_buffer();
}

//
void Update_render()
{
}

//
void Update_keyboard(char key)
{
    JsonDocument &app = status();

    // Clear Screen
    epd_poweron();
    epd_clear_quick(epd_full_screen(), 4, 50);
    epd_poweroff_all();

    //
    int cursorX = 10;
    int cursorY = 150;
    writeln(
        (GFXfont *)&systemFont,
        "UPDATING ...",
        &cursorX, &cursorY,
        display_EPD_framebuffer());

    // render frambuffer
    display_draw_buffer();

    //
    _log("Update update begins.\n");
    run_firmare_update(FIRMWARE);
}
