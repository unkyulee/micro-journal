#include "sd.h"
#include "app/app.h"
#include "display/display.h"
#include "display/Menu/Menu.h"

//
#include <SPI.h>
#include <FS.h>
#include <SD.h>
#include <SPIFFS.h>

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

    // Check if there are firmware.bin in the SD card
#ifdef ENV_USBHOST
        const char *firmware_filename = "/firmware_rev_5.bin";
#endif
#ifdef ENV_KEYBOARD
        const char *firmware_filename = "/firmware_rev_6.bin";
#endif
    if (SD.exists(firmware_filename))
    {
        app["screen"] = MENUSCREEN;
        app["menu"]["state"] = MENU_FIRMWARE;
    }
}
