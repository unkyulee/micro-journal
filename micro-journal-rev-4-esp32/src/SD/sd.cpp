#include "sd.h"
#include "app/app.h"
#include "display/display.h"

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
#ifdef ENV_EPAPER
    SPI.begin(SD_SCLK, SD_MISO, SD_MOSI, SD_CS);
    if (!SD.begin(SD_CS, SPI))
#else
    if (!SD.begin(SD_CS))
#endif
    {
        //
        app_log("Card Mount Failed\n");

        //
        app["error"] = " SD CARD MISSING ";
        app["screen"] = ERRORSCREEN;

        return;
    }

    // Check for card type
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

    // Create an empty log.txt file
    File logFile = SD.open("/log.txt", FILE_WRITE);
    logFile.close();
    
    // Check if there are firmware.bin in the SD card
#ifdef ENV_USBHOST
    const char *firmware_filename = "/firmware_rev_5.bin";
#endif
#ifdef ENV_KEYBOARD
    const char *firmware_filename = "/firmware_rev_6.bin";
#endif
#ifdef ENV_EPAPER
    const char *firmware_filename = "/firmware_rev_7.bin";
#endif

    if (SD.exists(firmware_filename))
    {
        app["screen"] = MENUSCREEN;
        app["menu"]["state"] = MENU_FIRMWARE;
    }


}
