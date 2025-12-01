#pragma once
// use it only for ESP32
#ifdef BOARD_ESP32_S3

#include "app/FileSystem/FileSystem.h"
#include "app/app.h"
#include <SD.h>

class FileSystemSD : public FileSystem
{
public:
    bool begin() override
    {
#if defined(SD_MOSI)
        // Lilygo T5 has SD card configuration
        _log("SD Card Init: SCLK %d MISO %d MOSI %d CS %d\n",
             SD_SCLK, SD_MISO, SD_MOSI, SD_CS);
        SPI.begin(SD_SCLK, SD_MISO, SD_MOSI);
        return SD.begin(SD_CS, SPI, SPI_FREQUENCY);
#else
        return SD.begin(SD_CS);
#endif
    }
    File open(const char *path, const char *mode) override
    {
        return SD.open(path, mode);
    }
    bool exists(const char *path) override { return SD.exists(path); }
    bool remove(const char *path) override { return SD.remove(path); }
    bool rename(const char *pathFrom, const char *pathTo) override
    {
        return SD.rename(pathFrom, pathTo);
    }
};

#endif