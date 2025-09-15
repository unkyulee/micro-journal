#include "Updater.h"
#include "app/app.h"
#include "display/display.h"

#include <Update.h> // Required for firmware update

void run_firmare_update(const char* filename)
{
    //
    JsonDocument &app = status();

    //
    _log("[run_firmare_update] Firmware update: %s\n", filename);

    //
    File firmwareFile = gfs()->open(filename, "r");
    if (!firmwareFile)
    {
        //
        app["error"] = format("ERROR OPENING: %s\n", filename);
        app["screen"] = ERRORSCREEN;

        _log(app["error"]);        

        return;
    }

    // Perform firmware update
    size_t fileSize = firmwareFile.size();
    if (Update.begin(fileSize))
    {
        if (Update.writeStream(firmwareFile) == fileSize)
        {
            if (Update.end())
            {
                Serial.println("Firmware update successful!");
                gfs()->remove(filename);

                // restart
                ESP.restart();
            }
            else
            {
                Update.printError(Serial);
                Serial.println("Error ending update!");

                //
                app["error"] = "UPDATE FAILED AT THE END";
                app["screen"] = ERRORSCREEN;
            }
        }
        else
        {
            Update.printError(Serial);
            Serial.println("Error writing firmware!");

            //
            app["error"] = "UPDATE FAILED TO APPLY";
            app["screen"] = ERRORSCREEN;
        }
    }
    else
    {
        Update.printError(Serial);
        Serial.println("Error beginning update!");

        //
        app["error"] = "UPDATE FAILED TO BEGIN";
        app["screen"] = ERRORSCREEN;
    }

    // Close firmware file
    firmwareFile.close();
    delay(100);
}
