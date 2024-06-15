#include "sd.h"
#include "app/app.h"
#include "display/display.h"

//
#include <SPI.h>
#include <FS.h>
#include <SD.h>
#include <SPIFFS.h>
#include <Update.h> // Required for firmware update

#define FORMAT_SPIFFS_IF_FAILED true

//
void SD_setup()
{
    //
    JsonDocument &app = app_status();

    // Mount SPIFFS
    if (!SPIFFS.begin())
    {
        //
        app_log("SPIFFS mount failed!\n");

        //
        app["error"] = "ESP32 NOT FORMATTED";
        app["screen"] = ERRORSCREEN;

        return;
    }
    else
    {
        app_log("SPIFFS mount succeeded!\n");
    }

    // initialize SD card
    app_log("SD Device CS: %d\n", SD_CS);
    if (!SD.begin(SD_CS))
    {
        //
        app_log("Card Mount Failed\n");

        //
        app["error"] = " SD CARD MISSING ";
        app["screen"] = ERRORSCREEN;

        return;
    }
    app_log("SD Device Initialized\n");
    uint8_t cardType = SD.cardType();

    if (cardType == CARD_NONE)
    {
        //
        app_log("No SD card attached\n");
        //
        app["error"] = " SD CARD NOT DETECTED ";
        app["screen"] = ERRORSCREEN;

        return;
    }

    //
    app_log("SD Card detected\n");

    // check firmware update
    SD_firwamre_update();
}


void SD_firwamre_update()
{
    //
    JsonDocument &app = app_status();

    // Check if there are firmware.bin in the SD card
    if (SD.exists("/firmware.bin"))
    {
        //
        app_log("Firmware update begins.\n");

        // perform update
        File firmwareFile = SD.open("/firmware.bin");
        if (!firmwareFile)
        {
            //
            Serial.println("Error opening firmware file!");

            //
            app["error"] = "ERROR FIRMWARE.BIN";
            app["screen"] = ERRORSCREEN;

            return;
        }

        // delay before next file operations
        delay(100);

        // Perform firmware update
        size_t fileSize = firmwareFile.size();
        if (Update.begin(fileSize))
        {
            if (Update.writeStream(firmwareFile) == fileSize)
            {
                if (Update.end())
                {
                    Serial.println("Firmware update successful!");
                    // Delete firmware file from SD card (optional)
                    SD.remove("/firmware.bin");
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
    }
}