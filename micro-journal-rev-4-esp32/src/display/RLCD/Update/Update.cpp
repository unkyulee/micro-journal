#include "Update.h"
#include "app/app.h"
#include "display/display.h"
#include "service/Editor/Editor.h"

//
#include "service/Updater/Updater.h"

//
void Update_setup(ST7305_4p2_BW_DisplayDriver *display, U8G2_FOR_ST73XX *u8)
{    
    _log("Update Screen Setup\n");
    display->clearDisplay();
}

//
void Update_render(ST7305_4p2_BW_DisplayDriver *display, U8G2_FOR_ST73XX *u8)
{
    u8->setCursor(0, 30);

    //
    u8->println();
    u8->println("Press any key to proceed with the update");

    u8->println();
    u8->println("While the update is on-going screen may flicker for a minute");
    u8->println();
}

//
void Update_keyboard(char key)
{
    run_firmare_update(FIRMWARE);
}
