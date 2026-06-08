#pragma once

#if defined(USE_MSC)

#include "app/FileSystem/FileSystem.h"
#include "app/app.h"

#include <Arduino.h>
#include <FS.h>
#include <FFat.h>

class FileSystemFAT : public FileSystem
{
public:
    FileSystemFAT(
        const char *partitionLabel = "storage",
        const char *basePath = "/fat",
        bool formatOnFail = true,
        uint8_t maxOpenFiles = 10)
        : _partitionLabel(partitionLabel),
          _basePath(basePath),
          _formatOnFail(formatOnFail),
          _maxOpenFiles(maxOpenFiles),
          _mounted(false)
    {
    }

    bool begin() override
    {
        if (_mounted)
        {
            return true;
        }

        _log("FAT Flash Init: partition=%s basePath=%s\n",
             _partitionLabel,
             _basePath);

        bool ok = FFat.begin(
            _formatOnFail,
            _basePath,
            _maxOpenFiles,
            _partitionLabel);

        if (!ok)
        {
            _log("FAT Flash Init Failed\n");
            _mounted = false;
            return false;
        }

        _mounted = true;

        _log("FAT Flash Init OK: total=%llu used=%llu\n",
             static_cast<unsigned long long>(FFat.totalBytes()),
             static_cast<unsigned long long>(FFat.usedBytes()));

        ensureReadyFile();

        return true;
    }

    void end() override
    {
        if (!_mounted)
        {
            return;
        }

        FFat.end();
        _mounted = false;

        _log("FAT Flash End\n");
    }

    bool mounted() const
    {
        return _mounted;
    }

    bool format()
    {
        bool wasMounted = _mounted;

        if (wasMounted)
        {
            end();
        }

        _log("FAT Flash Format: partition=%s\n", _partitionLabel);

        bool ok = FFat.format(false, const_cast<char *>(_partitionLabel));

        if (!ok)
        {
            _log("FAT Flash Format Failed\n");
            return false;
        }

        _log("FAT Flash Format OK\n");

        if (wasMounted)
        {
            return begin();
        }

        return true;
    }

    bool ensureReadyFile()
    {
        if (!_mounted)
        {
            return false;
        }

        if (FFat.exists("/ready.txt"))
        {
            return true;
        }

        File file = FFat.open("/ready.txt", FILE_WRITE);

        if (!file)
        {
            _log("FAT Flash ready.txt create failed\n");
            return false;
        }

        file.println("ESP32-S3 FAT storage ready");
        file.close();

        _log("FAT Flash ready.txt created\n");

        return true;
    }

    File open(const char *path, const char *mode) override
    {
        if (!_mounted)
        {
            _log("FAT Flash open failed, not mounted: %s\n", path);
            return File();
        }

        return FFat.open(path, mode);
    }

    bool exists(const char *path) override
    {
        if (!_mounted)
        {
            return false;
        }

        return FFat.exists(path);
    }

    bool remove(const char *path) override
    {
        if (!_mounted)
        {
            return false;
        }

        return FFat.remove(path);
    }

    bool rename(const char *pathFrom, const char *pathTo) override
    {
        if (!_mounted)
        {
            return false;
        }

        return FFat.rename(pathFrom, pathTo);
    }

    size_t totalBytes()
    {
        if (!_mounted)
        {
            return 0;
        }

        return FFat.totalBytes();
    }

    size_t usedBytes()
    {
        if (!_mounted)
        {
            return 0;
        }

        return FFat.usedBytes();
    }

private:
    const char *_partitionLabel;
    const char *_basePath;
    bool _formatOnFail;
    uint8_t _maxOpenFiles;
    bool _mounted;
};

#endif