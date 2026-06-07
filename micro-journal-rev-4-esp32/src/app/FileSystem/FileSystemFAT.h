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
        uint8_t maxOpenFiles = 10
    )
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
            _partitionLabel
        );

        if (!ok)
        {
            _log("FAT Flash Init Failed\n");
            _mounted = false;
            return false;
        }

        _mounted = true;

        _log("FAT Flash Init OK: total=%llu used=%llu\n",
             FFat.totalBytes(),
             FFat.usedBytes());

        return true;
    }

    void end()
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