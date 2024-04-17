#include "sd.h"
#include "app/app.h"
//
#include <SPI.h>
#include <FS.h>
#include <SD.h>

#ifdef USBHOST
#define SD_CS 9
#else
#define SD_CS 5
#endif

//
void SD_setup()
{
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

    // mark the status that SD card is ready
    JsonDocument &app = app_status();
    app["SD"] = true;
}

///
void SD_loop() {}
