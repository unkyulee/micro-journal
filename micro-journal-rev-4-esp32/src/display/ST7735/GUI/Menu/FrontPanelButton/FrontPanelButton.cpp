#include "FrontPanelButton.h"
#include "../Menu.h"
#include "app/app.h"
#include "display/display.h"
#include "../../WordProcessor/WordProcessor.h"

//
void FrontPanelButton_setup(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    // when entering the screen
    // clear the screen
    Menu_clear();
}

//
void FrontPanelButton_render(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    ptft->setCursor(0, 30, 2);
    ptft->setTextSize(1);
    //
    ptft->setTextColor(TFT_WHITE, TFT_BLACK);
    ptft->println(" FRONTPANEL BUTTONS");
    ptft->println("");

    //
    JsonDocument &app = app_status();
    bool disabled = app["config"]["front_panel_button_disabled"].as<bool>();

    if (disabled)
    {
        ptft->setTextColor(TFT_WHITE, TFT_RED);
        ptft->println("DISABLED");
        ptft->setTextColor(TFT_WHITE, TFT_BLACK);
        ptft->println("");
    }

    // 
    ptft->println(" [Y] Enable Front Panel Buttons ");
    ptft->println(" [N] Disable Front Panel Buttons ");

    //
    ptft->println();
    ptft->println(" [B] BACK ");
}

//
void FrontPanelButton_keyboard(char key)
{
    JsonDocument &app = app_status();

    if (key == 'y')
    {
        //
        app["config"]["front_panel_button_disabled"] = false;

        // 
        app_config_save();

        return;
    }
    else if (key == 'n')
    {
        //
        app["config"]["front_panel_button_disabled"] = true;

        //
        app_config_save();

        return;
    }

    // go back to home menu
    //
    app["menu"]["state"] = MENU_HOME;
}
