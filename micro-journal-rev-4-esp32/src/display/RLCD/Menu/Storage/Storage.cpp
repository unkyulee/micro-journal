#include "Storage.h"
#include "../Menu.h"
#include "app/app.h"
#include "display/display.h"

//
void Storage_setup(ST7305_4p2_BW_DisplayDriver *display, U8G2_FOR_ST73XX *u8)
{
    //
    Menu_clear();
    
    // Turn on Storage
    JsonDocument &app = status();
    app["massStorage"] = true;
}

//
void Storage_render(ST7305_4p2_BW_DisplayDriver *display, U8G2_FOR_ST73XX *u8)
{
    u8->setCursor(10, 50);    
    u8->println("Drive Mode");
    u8->setCursor(10, 75);    
    u8->println("Connect USB to PC");
    u8->setCursor(10, 100);    
    u8->println("Press ESC to Exit");
}

//
void Storage_keyboard(char key)
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

                //
                delay(3000);

                // restart
                ESP.restart();                

                //
                break;
            }

            delay(1000);
        }

        // Move to home screen
        app["menu"]["state"] = MENU_HOME;
    }
}