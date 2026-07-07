#include "Update.h"
#include "app/app.h"
#include "display/display.h"
#include "service/Editor/Editor.h"

//
#include "service/Updater/Updater.h"

// Whether the last Update_render() actually changed anything visible - the
// caller uses this to decide whether the (expensive, full 30KB SPI) panel
// refresh is worth doing this tick.
static bool needsDisplay = true;
static bool rendered_once = false;

//
void Update_setup(ST7305_4p2_BW_DisplayDriver *display, U8G2_FOR_ST73XX *u8)
{
    _log("Update Screen Setup\n");
    display->clearDisplay();

    // the screen content is static, so it only needs to be pushed once
    rendered_once = false;
}

//
void Update_render(ST7305_4p2_BW_DisplayDriver *display, U8G2_FOR_ST73XX *u8)
{
    u8->setFont(u8g2_font_profont17_tf);
    u8->setCursor(0, 30);

    //
    u8->println();
    u8->println("Press any key to proceed with the update");

    u8->println();
    u8->println("While the update is on-going screen may flicker for a minute");
    u8->println();

    // decide whether the panel actually needs to be pushed over SPI this tick
    needsDisplay = !rendered_once;
    rendered_once = true;
}

bool Update_needsDisplay()
{
    return needsDisplay;
}

//
void Update_keyboard(char key)
{
    run_firmare_update(FIRMWARE);
}
