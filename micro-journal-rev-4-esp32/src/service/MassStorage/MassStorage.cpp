#include "MassStorage.h"
#include "app/app.h"

#include <FatFSUSB.h>

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

        //
        bool massStorage = app["massStorage"].as<bool>();

        // mass storage is on
        if (massStorage)
        {
            // start the device
            if (app["massStorageStarted"].as<bool>() == false)
            {
                FatFSUSB.begin();

                //
                app["massStorageStarted"] = true;
                _log("FatFSUSB begin\n");
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