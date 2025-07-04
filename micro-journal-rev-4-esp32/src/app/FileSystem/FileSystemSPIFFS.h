// use it only for ESP32
#ifdef BOARD_ESP32_S3

#include "app/FileSystem/FileSystem.h"
#include "app/app.h"

#include <SPIFFS.h>

class FileSystemSPIFFS : public FileSystem
{
public:
    bool begin() override
    {
        return SPIFFS.begin();
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