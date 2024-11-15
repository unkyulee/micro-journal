#include "Firmware.h"
#include "../Menu.h"
#include "app/app.h"
#include "config/config.h"
#include "display/display.h"
#include "editor/editor.h"
#include <display/EPD/display_EPD.h>

//
#include <Update.h> // Required for firmware update
#include <FS.h>
#include <SD.h>
#include <SPIFFS.h>

//
void Firmware_setup()
{
}

//
void Firmware_render()
{
    //
    int cursorX = 10;
    int cursorY = 150;
    writeln(
        (GFXfont *)&systemFont,
        " Press any key to proceed with the firmware update",
        &cursorX, &cursorY,
        display_EPD_framebuffer());
}

//
void Firmware_keyboard(char key)
{
    JsonDocument &app = app_status();

    //
    app_log("Firmware update begins.\n");

    // perform update
#ifdef ENV_USBHOST
    const char *firmware_filename = "/firmware_rev_5.bin";
#endif
#ifdef ENV_KEYBOARD
    const char *firmware_filename = "/firmware_rev_6.bin";
#endif
#ifdef ENV_EPAPER
    const char *firmware_filename = "/firmware_rev_7.bin";
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
