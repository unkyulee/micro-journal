#pragma once

//
#include <Arduino.h>
#include <FS.h>

#ifdef BOARD_ESP32_S3
#include <SD.h>
#endif

class FileSystem {
public:
    virtual bool begin() = 0;
    virtual File open(const char* path, const char* mode) = 0;
    virtual bool exists(const char* path) = 0;
    virtual bool remove(const char* path) = 0;
    virtual bool rename(const char* pathFrom, const char* pathTo) = 0;
    virtual ~FileSystem() = default;
};