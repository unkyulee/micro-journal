#include "MassStorage.h"
#include "app/app.h"

#include <FatFSUSB.h>

bool massStorageStarted = false;

//
void ms_setup()
{
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
        if (massStorage)
        {
            if (massStorageStarted == false)
            {
                FatFSUSB.begin();

                //
                massStorageStarted = true;
                _log("FatFSUSB begin\n");
            }
        }
        else
        {
            if (massStorageStarted == true)
            {
                FatFSUSB.unplug();
                FatFSUSB.end();
                massStorageStarted = false;

                _log("FatFSUSB end\n");
            }
        }
    }
}