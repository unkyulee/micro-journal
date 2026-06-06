#pragma once

#ifdef BOARD_ESP32_S3

#include "app/FileSystem/FileSystem.h"
#include <SPIFFS.h>

class FileSystemSPIFFS : public FileSystem
{
public:
    bool begin() override
    {
#ifdef SPIFFS_FORMAT_ON_FAIL
        return SPIFFS.begin(true);
#else
        return SPIFFS.begin(false);
#endif
    }

    File open(const char *path, const char *mode) override
    {
        return SPIFFS.open(path, mode);
    }

    bool exists(const char *path) override { return SPIFFS.exists(path); }
    bool remove(const char *path) override { return SPIFFS.remove(path); }
    bool rename(const char *pathFrom, const char *pathTo) override
    {
        return SPIFFS.rename(pathFrom, pathTo);
    }
};

#endif
