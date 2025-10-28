#include "Verification.h"
#include "app/app.h"
#include "display/display.h"

#ifdef BOARD_ESP32_S3
#include <SD.h>
#endif

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
        _log("New firmware found. Preparing Update.\n");
        return true;
    }
#endif

    _log("No new firmware found.\n");
    return false;
}

// check if file system is correctly loaded
bool filesystem_check()
{
    //
    JsonDocument &app = status();

    // Call File System for the first time to initialize
    gfs();

#ifdef BOARD_ESP32_S3
    // Check if SD card is inserted
    uint8_t cardType = SD.cardType();
    if (cardType == CARD_NONE)
    {
        //
        app["error"] = "SD CARD NOT INSERTED\n";
        app["screen"] = ERRORSCREEN;

        //
        _log(app["error"]);

        return false;
    }

    //
    _log("SD Card detected\n");
#endif

    // file system check pass
    return true;
}
