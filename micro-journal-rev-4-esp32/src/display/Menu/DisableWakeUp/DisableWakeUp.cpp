#include "DisableWakeUp.h"
#include "../Menu.h"
#include "app/app.h"
#include "config/config.h"
#include "display/display.h"
#include "display/WordProcessor/WordProcessor.h"

//
void DisableWakeUp_setup(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    // when entering the screen
    // DisableWakeUp the screen
    Menu_clear();
}

//
void DisableWakeUp_render(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    ptft->setCursor(0, 30, 2);
    ptft->setTextSize(1);
    //
    ptft->setTextColor(TFT_WHITE, TFT_BLACK);
    ptft->println(" WAKEUP ANIMATION");
    ptft->println("");

    //
    JsonDocument &app = app_status();
    bool disabled = app["config"]["wakeup_animation_disabled"].as<bool>();

    if (disabled)
    {
        ptft->setTextColor(TFT_WHITE, TFT_RED);
        ptft->println("DISABLED");
        ptft->setTextColor(TFT_WHITE, TFT_BLACK);
        ptft->println("");
    }

    //
    ptft->println(" [Y] Enable Start Animation ");
    ptft->println(" [N] Disable Start Animation ");

    //
    ptft->println();
    ptft->println(" [B] BACK ");
}

//
void DisableWakeUp_keyboard(char key)
{
    JsonDocument &app = app_status();

    if (key == 'y')
    {
        //
        app["config"]["wakeup_animation_disabled"] = false;

        // 
        config_save();

        return;
    }
    else if (key == 'n')
    {
        //
        app["config"]["wakeup_animation_disabled"] = true;

        //
        config_save();

        return;
    }

    // go back to home menu
    //
    app["menu"]["state"] = MENU_HOME;
}
