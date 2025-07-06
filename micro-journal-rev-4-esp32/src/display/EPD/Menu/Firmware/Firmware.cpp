#include "Firmware.h"
#include "../Menu.h"
#include "app/app.h"
#include "display/display.h"
#include <display/EPD/display_EPD.h>

//
#include "service/Updater/Updater.h"

//
void Firmware_setup()
{
}

//
void Firmware_render()
{
    //
    int cursorX = 10;
    int cursorY = 150;
    writeln(
        (GFXfont *)&systemFont,
        " Press any key to proceed with the firmware update",
        &cursorX, &cursorY,
        display_EPD_framebuffer());
}

//
void Firmware_keyboard(char key)
{
    JsonDocument &app = status();

    //
    _log("Firmware update begins.\n");
    run_firmare_update(FIRMWARE);
}
