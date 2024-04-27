#include "sd.h"
#include "app/app.h"
//
#include <SPI.h>
#include <FS.h>
#include <SD.h>
#include <SPIFFS.h>
#include <Update.h> // Required for firmware update

//
void SD_setup()
{
    // Mount SPIFFS
    if (!SPIFFS.begin())
    {
        Serial.println("SPIFFS mount failed!");
        return;
    }
    else
    {
        Serial.println("SPIFFS mount succeeded!");
    }

    // initialize SD card
    app_log("SD Device CS: %d\n", SD_CS);
    if (!SD.begin(SD_CS))
    {
        Serial.println("Card Mount Failed");
        return;
    }
    app_log("SD Device Initialized\n");
    uint8_t cardType = SD.cardType();

    if (cardType == CARD_NONE)
    {
        Serial.println("No SD card attached");
        return;
    }
    Serial.println("SD Card detected");

    // check firmware update
    SD_firwamre_update();

    // mark the status that SD card is ready
    JsonDocument &app = app_status();
    app["SD"] = true;
}

///
void SD_loop() {}

void SD_firwamre_update()
{
    // Check if there are firmware.bin in the SD card
    if (SD.exists("/firmware.bin"))
    {
        //
        app_log("Firmware update begins.\n");

        // perform update
        File firmwareFile = SD.open("/firmware.bin");
        if (!firmwareFile)
        {
            Serial.println("Error opening firmware file!");
            return;
        }

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
                }
            }
            else
            {
                Update.printError(Serial);
                Serial.println("Error writing firmware!");
            }
        }
        else
        {
            Update.printError(Serial);
            Serial.println("Error beginning update!");
        }

        // Close firmware file
        firmwareFile.close();
    }
}