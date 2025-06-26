#pragma once
#include "app/FileSystem/FileSystem.h"
#include <LittleFS.h>

class FSLittleFS : public FileSystem {
public:
    bool begin() override { return LittleFS.begin(); }
    File open(const char* path, const char* mode) override { return LittleFS.open(path, mode); }
    bool exists(const char* path) override { return LittleFS.exists(path); }
    bool remove(const char* path) override { return LittleFS.remove(path); }
};