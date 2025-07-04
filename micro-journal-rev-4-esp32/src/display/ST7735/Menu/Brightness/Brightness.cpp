#include "Brightness.h"
#include "../Menu.h"
#include "app/app.h"
#include "display/display.h"

//
void Brightness_setup(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    //
    Menu_clear();
}

//
void Brightness_render(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    JsonDocument &app = status();
    int brightness = app["config"]["brightness"].as<int>();

    ptft->setCursor(0, 0);
    ptft->setTextSize(2);
    ptft->setTextColor(TFT_WHITE, TFT_BLACK);

    ptft->println(""); // Blank line if out of bounds
    ptft->println("Brightness");
    ptft->println("");
    ptft->print("  ");
    for (int i = 0; i < brightness; i++)
    {
        ptft->print("=");
    }
}

//
void Brightness_keyboard(char key, bool pressed)
{
    if (pressed)
        return;

    //
    _debug("Brightness_Keyboard %d\n", key);
    JsonDocument &app = status();

    // MENU - SELECTED ACTION
    if (key == 6 || key == '\n')
    {
        // Go back to Home
        _log("Exit Brightness");

        // Save Config
        config_save();

        //
        app["menu"]["state"] = MENU_HOME;
    }

    // UP
    else if (key == 21)
    {
        int brightness = app["config"]["brightness"].as<int>();
        brightness++;
        if (brightness > 10)
            brightness = 10;

        app["config"]["brightness"] = brightness;
    }

    // DOWN
    else if (key == 20)
    {
        int brightness = app["config"]["brightness"].as<int>();
        brightness--;
        if (brightness < 0)
            brightness = 0;

        app["config"]["brightness"] = brightness;
    }
}
