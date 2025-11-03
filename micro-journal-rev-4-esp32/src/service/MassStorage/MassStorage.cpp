#include "MassStorage.h"
#include "app/app.h"
#include "FatFSUSB.h"

//
void ms_setup()
{
    // Register callback for host ejection
    _log("Mass Storage Setup\n");
}

//
void ms_loop()
{
    static unsigned int last = millis();
    if (millis() - last > 1000)
    {
        last = millis();

        // check if the flag is on
        JsonDocument &app = status();

        // check if it is requested to turn off
        bool massStorageRequest = app["massStorage"].as<bool>();
        // false means it is requested to turn off

        // mass storage is on
        if (massStorageRequest)
        {
            // start the device
            if (app["massStorageStarted"].as<bool>() == false)
            {
                FatFSUSB.begin();

                //
                app["massStorageStarted"] = true;
                _log("FatFSUSB begin\n");
            }

            // Check if is Ejected
            if (FatFSUSB.isConnected() == 0)
            {
                //
                _log("Mass Storage Ejected\n");
                FatFSUSB.end();
                //
                app["massStorageStarted"] = false;
                app["massStorage"] = false;

                _log("FatFSUSB ejected\n");
            }
        }

        else
        {
            if (app["massStorageStarted"].as<bool>() == true)
            {
                FatFSUSB.unplug();
                FatFSUSB.end();

                //
                app["massStorageStarted"] = false;
                _log("FatFSUSB end\n");
            }
        }
    }
}