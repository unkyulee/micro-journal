#pragma once

#ifdef BOARD_ESP32_S3

#include <Arduino.h>

#include "USB.h"
#include "USBMSC.h"

#include "esp_partition.h"
#include "wear_levelling.h"

void ms_esp32_loop();

class FatFSUSBClass
{
public:
    bool begin();
    void end();
    void unplug();
    bool isConnected();

private:
    static int32_t readCallback(
        uint32_t lba,
        uint32_t offset,
        void *buffer,
        uint32_t bufsize
    );

    static int32_t writeCallback(
        uint32_t lba,
        uint32_t offset,
        uint8_t *buffer,
        uint32_t bufsize
    );

    static bool startStopCallback(
        uint8_t power_condition,
        bool start,
        bool load_eject
    );

private:
    static constexpr uint16_t BLOCK_SIZE = 512;

    static USBMSC msc;
    static wl_handle_t wlHandle;

    static bool usbStarted;
    static bool mediaStarted;
    static bool ejected;

    static uint32_t blockCount;
};

extern FatFSUSBClass FatFSUSB;



#endif