#include "Verification.h"
#include "app/app.h"
#include "display/display.h"

// check firmware update
bool firmware_check()
{
#ifdef BOARD_ESP32_S3
    // load app status
    JsonDocument &app = status();

    // Check if there are firmware.bin in the SD card
    // FIRMWARE is defiined in platformio.ini
    _log("Checking for firmware update: %s\n", FIRMWARE);
    if (gfs()->exists(FIRMWARE))
    {
        // move to firmware update screen
        app["screen"] = UPDATESCREEN;

        return true;
    }
#endif

    return false;
}

// check if file system is correctly loaded
bool filesystem_check()
{
    //
    JsonDocument &app = status();

#ifdef BOARD_ESP32_S3
    // CHECK IF SPIFFS is formatted
    // Mount SPIFFS
    if (!SPIFFS.begin())
    {
        //
        _log("SPIFFS mount failed!\n");

        //
        app["error"] = "SPIFFS NOT FORMATTED";
        app["screen"] = ERRORSCREEN

            return false;
    }

    // Initialize SD Card
#if defined(ENV_EPAPER)
    //
    SPI.begin(SD_SCLK, SD_MISO, SD_MOSI, SD_CS);
    if (!SD.begin(SD_CS, SPI))
#else
    //
    if (!SD.begin(SD_CS))
#endif
    {
        //
        _log("Card Mount Failed\n");

        //
        app["error"] = " SD CARD MISSING ";
        app["screen"] = ERRORSCREEN;

        return false;
    }

    // Check for card type
    _log("SD Device Initialized\n");
    uint8_t cardType = SD.cardType();

    if (cardType == CARD_NONE)
    {
        //
        _log("No SD card attached\n");
        //
        app["error"] = " SD CARD NOT DETECTED ";
        app["screen"] = ERRORSCREEN;

        return false;
    }

    //
    _log("SD Card detected\n");
#endif

    // file system check pass
    return true;
}
