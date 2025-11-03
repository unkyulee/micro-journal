#include "Storage.h"
#include "../Menu.h"
#include "app/app.h"
#include "display/display.h"

//
#include "hardware/watchdog.h"

//
void Storage_setup(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    //
    Menu_clear();

    // Turn on Storage
    JsonDocument &app = status();
    app["massStorage"] = true;
}

//
void Storage_render(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    ptft->setCursor(0, 0);
    ptft->setTextSize(2);
    ptft->setTextColor(TFT_WHITE, TFT_BLACK);

    ptft->println(""); // Blank line if out of bounds
    ptft->println("Drive Mode");
    ptft->setTextSize(1);
    ptft->println("");
    ptft->println("Connect USB to PC");
    ptft->println("Press ESC to Exit");
}

//
void Storage_keyboard(char key, bool pressed)
{
    _debug("Storage_Keyboard %d\n", key);
    JsonDocument &app = status();

    // MENU - SELECTED ACTION
    if (key == 27)
    {
        // Go back to Home
        _log("Exit Mass Storage Started\n");

        // turn off USB drive
        app["massStorage"] = false;

        // wait until the storage is off
        while (true)
        {
            //
            _log("Checking when the device is ejected\n");

            //
            if (app["massStorageStarted"].as<bool>() == false)
            {
                _log("Detected device is ejected. Rebooting\n");

                // RESTART
                watchdog_reboot(0, 0, 3000);

                //
                delay(3000);

                //
                break;
            }

            delay(1000);
        }

        // Move to home screen
        app["menu"]["state"] = MENU_HOME;
    }
}