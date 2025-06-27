#pragma once

#include "app/FileSystem/FileSystem.h"
#include <FatFS.h>
#include "app/app.h"

class FileSystemRP2040 : public FileSystem
{
public:
    bool begin() override
    {
        return FatFS.begin();
    }
    File open(const char *path, const char *mode) override
    {
        return FatFS.open(path, mode);
    }
    bool exists(const char *path) override { return FatFS.exists(path); }
    bool remove(const char *path) override { return FatFS.remove(path); }
    bool rename(const char *pathFrom, const char *pathTo) override
    {
        return FatFS.rename(pathFrom, pathTo);
    }
};