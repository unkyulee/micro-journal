#include "Firmware.h"
#include "../Menu.h"
#include "app/app.h"
#include "config/config.h"
#include "display/display.h"
#include "editor/editor.h"

//
#include <Update.h> // Required for firmware update
#include <FS.h>
#include <SD.h>
#include <SPIFFS.h>

//
void Firmware_setup(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    // when entering the screen
    // Firmware the screen
    Menu_clear();
}

//
void Firmware_render(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    ptft->setCursor(0, 30, 2);
    ptft->setTextSize(1);

    //
    ptft->setTextColor(TFT_WHITE, TFT_BLACK);
    ptft->println();
    ptft->setTextColor(TFT_WHITE, TFT_RED);
    ptft->println(" [Y] ARE YOU SURE TO UPDATE THE FIRMWARE?");

    ptft->setTextColor(TFT_WHITE, TFT_BLACK);
    ptft->println();
    ptft->println("While the update is on-going screen may flicker for a minute");
    ptft->println();    
    ptft->println(" [B] BACK ");
}

//
void Firmware_keyboard(char key)
{
    JsonDocument &app = app_status();
    // delete confirmed
    if (key == 'Y' || key == 'y')
    {
        //
        app_log("Firmware update begins.\n");

        // perform update
#ifdef ENV_USBHOST
        const char *firmware_filename = "/firmware_rev_5.bin";
#endif
#ifdef ENV_KEYBOARD
        const char *firmware_filename = "/firmware_rev_6.bin";
#endif

        File firmwareFile = SD.open(firmware_filename);
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
                    SD.remove(firmware_filename);

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

    // go back to word processor
    //
    app["menu"]["state"] = MENU_HOME;
    app["screen"] = WORDPROCESSOR;
}
