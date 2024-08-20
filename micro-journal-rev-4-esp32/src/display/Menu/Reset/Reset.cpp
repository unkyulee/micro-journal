#include "Reset.h"
#include "../Menu.h"
#include "app/app.h"
#include "config/config.h"
#include "display/display.h"
#include "editor/editor.h"

//
void Reset_setup(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    // when entering the screen
    // Reset the screen
    Menu_clear();
}

//
void Reset_render(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    ptft->setCursor(0, 30, 2);
    ptft->setTextSize(1);

    //
    ptft->setTextColor(TFT_WHITE, TFT_BLACK);
    ptft->println();
    ptft->setTextColor(TFT_WHITE, TFT_RED);
    ptft->println(" [Y] ARE YOU SURE TO RESET THE DEVICE?");

    ptft->setTextColor(TFT_WHITE, TFT_BLACK);
    ptft->println();
    ptft->println("WARNING: This action will delete the wifi configuration file.");
    ptft->println();
    ptft->println(" [B] BACK ");
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
