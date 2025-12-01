#include "Update.h"
#include "app/app.h"
#include "display/display.h"
#include "service/Editor/Editor.h"

//
#include "service/Updater/Updater.h"

//
#include "display/CARDPUTER/display_CARDPUTER.h"

//
void Update_setup()
{
    //
    _log("Update Screen Setup\n");

    // when clearing background
    M5Cardputer.Display.fillRect(
        0,
        0,
        M5Cardputer.Display.width(),
        M5Cardputer.Display.height(),
        TFT_BLACK);
}

//
void Update_render()
{
    ///
    M5Cardputer.Display.setTextSize(1); // Adjust text size as needed
    M5Cardputer.Display.setTextColor(TFT_WHITE, TFT_RED);
    M5Cardputer.Display.setFont(&fonts::FreeMono18pt7b);

    // Draw text in the middle of the screen
    drawWrappedText("Press any key to proceed with the update. While the update is on-going screen may flicker for a minute", 0, 0, M5Cardputer.Display.width());
}

//
void Update_keyboard(char key)
{
    run_firmare_update(FIRMWARE);
}
