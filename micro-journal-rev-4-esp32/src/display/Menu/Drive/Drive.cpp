#include "Drive.h"
#include "../Menu.h"
#include "app/app.h"
#include "config/config.h"
#include "display/display.h"
#include "display/WordProcessor/WordProcessor.h"

//
void Drive_setup(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    // when entering the screen
    // clear the screen
    Menu_clear();
}

//
void Drive_render(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    ptft->setCursor(0, 30, 2);
    ptft->setTextSize(1);

    //
    ptft->setTextColor(TFT_WHITE, TFT_BLACK);
    ptft->println(" PRESS ANY KEY TO RESTART AS A USB DRIVE");
    
    ptft->println();
    ptft->println(" [B] BACK ");
}

//
void Drive_keyboard(char key)
{
    JsonDocument &app = app_status();

    // delete confirmed
    if (key == 'b' || key == 'B')
    {
        // go back to home menu
        //
        app["menu"]["state"] = MENU_HOME;
    }
    else
    {
        // boot as USB DRIVE
        app["config"]["drive"] = true;
        config_save();

        // reboot with USB DRIVE MODE
        ESP.restart();
    }
}