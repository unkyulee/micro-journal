#include "Update.h"
#include "app/app.h"
#include "display/display.h"
#include "service/Editor/Editor.h"

//
#include "service/Updater/Updater.h"

//
void Update_setup(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    ptft->fillScreen(TFT_BLACK);
    _log("Update Screen Setup\n");
}

//
void Update_render(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    ptft->setCursor(0, 30, 2);
    ptft->setTextSize(1);

    //
    ptft->setTextColor(TFT_WHITE, TFT_BLACK);
    ptft->println();
    ptft->setTextColor(TFT_WHITE, TFT_RED);
    ptft->println("Press any key to proceed with the update");

    ptft->setTextColor(TFT_WHITE, TFT_BLACK);
    ptft->println();
    ptft->println("While the update is on-going screen may flicker for a minute");
    ptft->println();
}

//
void Update_keyboard(char key)
{
    run_firmare_update(FIRMWARE);
}
