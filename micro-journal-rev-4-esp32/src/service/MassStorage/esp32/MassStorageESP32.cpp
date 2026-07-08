#ifdef BOARD_ESP32_S3

#include "app/app.h"
#include "MassStorageESP32.h"

USBMSC FatFSUSBClass::msc;
wl_handle_t FatFSUSBClass::wlHandle = WL_INVALID_HANDLE;
uint8_t *FatFSUSBClass::sectorBuffer = nullptr;

bool FatFSUSBClass::usbStarted = false;
bool FatFSUSBClass::mediaStarted = false;
bool FatFSUSBClass::ejected = false;

uint32_t FatFSUSBClass::blockCount = 0;
uint16_t FatFSUSBClass::blockSize = 0;
size_t FatFSUSBClass::mediaSize = 0;

FatFSUSBClass FatFSUSB;

//
void ms_esp32_loop()
{
    static unsigned int last = millis();

    if (millis() - last <= 1000)
    {
        return;
    }

    last = millis();

    JsonDocument &app = status();

    bool massStorageRequest = app["massStorage"].as<bool>();
    bool massStorageStarted = app["massStorageStarted"].as<bool>();

    if (massStorageRequest)
    {
        if (!massStorageStarted)
        {
            // disactivate FAT file system before Mass Storage
            gfs()->end();

            // full CPU speed for USB transfer
            // device is powered over USB during mass storage anyway
            setCpuFrequencyMhz(CPU_FREQUENCY_FULL);

            if (FatFSUSB.begin())
            {
                app["massStorageStarted"] = true;
                _log("FatFSUSB begin\n");
            }
            else
            {
                setCpuFrequencyMhz(CPU_FREQUENCY_LOW);

                app["massStorageStarted"] = false;
                app["massStorage"] = false;
                _log("FatFSUSB begin failed\n");
                return;
            }
        }

        if (FatFSUSB.isConnected() == 0)
        {
            _log("Mass Storage Ejected\n");

            FatFSUSB.end();

            app["massStorageStarted"] = false;
            app["massStorage"] = false;

            _log("Mass Storage ejected\n");

            delay(3000);
            ESP.restart();
        }
    }
    else
    {
        if (massStorageStarted)
        {
            FatFSUSB.unplug();
            FatFSUSB.end();

            app["massStorageStarted"] = false;
            _log("FatFSUSB end\n");

            // back to the battery saving CPU speed
            // the ejected path skips this since it restarts the device
            setCpuFrequencyMhz(CPU_FREQUENCY_LOW);
        }
    }
}

bool FatFSUSBClass::begin()
{
    if (mediaStarted)
    {
        return true;
    }

    const esp_partition_t *partition = esp_partition_find_first(
        ESP_PARTITION_TYPE_DATA,
        ESP_PARTITION_SUBTYPE_DATA_FAT,
        "storage");

    if (partition == nullptr)
    {
        Serial.println("MassStorageESP32: FAT partition 'storage' not found");
        return false;
    }

    esp_err_t err = wl_mount(partition, &wlHandle);

    if (err != ESP_OK)
    {
        Serial.printf("MassStorageESP32: wl_mount failed: %d\n", err);
        wlHandle = WL_INVALID_HANDLE;
        return false;
    }

    size_t wlSectorSize = wl_sector_size(wlHandle);

    if (wlSectorSize == 0 || wlSectorSize > UINT16_MAX)
    {
        Serial.printf(
            "MassStorageESP32: invalid WL sector size: %u\n",
            static_cast<unsigned>(wlSectorSize));

        wl_unmount(wlHandle);
        wlHandle = WL_INVALID_HANDLE;

        return false;
    }

    blockSize = static_cast<uint16_t>(wlSectorSize);
    mediaSize = wl_size(wlHandle);
    blockCount = mediaSize / blockSize;
    mediaSize = static_cast<size_t>(blockCount) * blockSize;

    if (blockCount == 0)
    {
        Serial.println("MassStorageESP32: invalid block count");

        wl_unmount(wlHandle);
        wlHandle = WL_INVALID_HANDLE;

        return false;
    }

    sectorBuffer = static_cast<uint8_t *>(malloc(blockSize));

    if (sectorBuffer == nullptr)
    {
        Serial.printf(
            "MassStorageESP32: sector buffer allocation failed: %u\n",
            blockSize);

        wl_unmount(wlHandle);
        wlHandle = WL_INVALID_HANDLE;
        blockCount = 0;
        blockSize = 0;
        mediaSize = 0;

        return false;
    }

    msc.vendorID("ESP32S3");
    msc.productID("FlashDisk");
    msc.productRevision("1.0");

    msc.onRead(readCallback);
    msc.onWrite(writeCallback);
    msc.onStartStop(startStopCallback);

    msc.mediaPresent(true);

    if (!msc.begin(blockCount, blockSize))
    {
        Serial.println("MassStorageESP32: MSC begin failed");

        free(sectorBuffer);
        sectorBuffer = nullptr;

        wl_unmount(wlHandle);
        wlHandle = WL_INVALID_HANDLE;
        blockCount = 0;
        blockSize = 0;
        mediaSize = 0;

        return false;
    }

    if (!usbStarted)
    {
        USB.begin();
        usbStarted = true;
    }

    mediaStarted = true;
    ejected = false;

    Serial.printf(
        "MassStorageESP32: started, blocks=%lu, blockSize=%u\n",
        static_cast<unsigned long>(blockCount),
        blockSize);

    return true;
}

void FatFSUSBClass::end()
{
    if (!mediaStarted)
    {
        return;
    }

    msc.mediaPresent(false);

    if (wlHandle != WL_INVALID_HANDLE)
    {
        wl_unmount(wlHandle);
        wlHandle = WL_INVALID_HANDLE;
    }

    free(sectorBuffer);
    sectorBuffer = nullptr;

    mediaStarted = false;
    ejected = false;
    blockCount = 0;
    blockSize = 0;
    mediaSize = 0;

    Serial.println("MassStorageESP32: ended");
}

void FatFSUSBClass::unplug()
{
    if (!mediaStarted)
    {
        return;
    }

    msc.mediaPresent(false);
    ejected = true;

    Serial.println("MassStorageESP32: media unplugged");
}

bool FatFSUSBClass::isConnected()
{
    return mediaStarted && !ejected;
}

int32_t FatFSUSBClass::readCallback(
    uint32_t lba,
    uint32_t offset,
    void *buffer,
    uint32_t bufsize)
{
    if (wlHandle == WL_INVALID_HANDLE)
    {
        return -1;
    }

    if (blockSize == 0 || mediaSize == 0 || offset > blockSize)
    {
        return -1;
    }

    const uint64_t address =
        static_cast<uint64_t>(lba) * blockSize + offset;

    if (address > mediaSize || bufsize > mediaSize - address)
    {
        return -1;
    }

    esp_err_t err = wl_read(
        wlHandle,
        static_cast<size_t>(address),
        buffer,
        bufsize);

    if (err != ESP_OK)
    {
        return -1;
    }

    return static_cast<int32_t>(bufsize);
}

int32_t FatFSUSBClass::writeCallback(
    uint32_t lba,
    uint32_t offset,
    uint8_t *buffer,
    uint32_t bufsize)
{
    if (wlHandle == WL_INVALID_HANDLE)
    {
        return -1;
    }

    if (blockSize == 0 ||
        mediaSize == 0 ||
        sectorBuffer == nullptr ||
        offset > blockSize)
    {
        return -1;
    }

    const uint64_t startAddress =
        static_cast<uint64_t>(lba) * blockSize + offset;

    if (startAddress > mediaSize || bufsize > mediaSize - startAddress)
    {
        return -1;
    }

    uint32_t remaining = bufsize;
    const uint8_t *source = buffer;
    size_t address = static_cast<size_t>(startAddress);

    while (remaining > 0)
    {
        const size_t sectorStart = (address / blockSize) * blockSize;
        const size_t sectorOffset = address - sectorStart;
        const size_t chunk = min(
            static_cast<size_t>(remaining),
            static_cast<size_t>(blockSize) - sectorOffset);

        esp_err_t err = wl_read(wlHandle, sectorStart, sectorBuffer, blockSize);

        if (err != ESP_OK)
        {
            return -1;
        }

        memcpy(sectorBuffer + sectorOffset, source, chunk);

        err = wl_erase_range(wlHandle, sectorStart, blockSize);

        if (err != ESP_OK)
        {
            return -1;
        }

        err = wl_write(wlHandle, sectorStart, sectorBuffer, blockSize);

        if (err != ESP_OK)
        {
            return -1;
        }

        remaining -= static_cast<uint32_t>(chunk);
        source += chunk;
        address += chunk;
    }

    return static_cast<int32_t>(bufsize);
}

bool FatFSUSBClass::startStopCallback(
    uint8_t power_condition,
    bool start,
    bool load_eject)
{
    (void)power_condition;

    // Only flag the eject here -- do NOT clear mediaStarted. end()'s
    // "if (!mediaStarted) return;" guard would otherwise see it already
    // false (this runs from the USB callback, ahead of ms_esp32_loop()
    // noticing isConnected() went false and calling end()) and skip its
    // real cleanup entirely, most importantly wl_unmount(), leaving the
    // wear-leveling layer's own mount state never cleanly released before
    // the subsequent reboot.
    if (load_eject && !start)
    {
        ejected = true;
    }

    return true;
}

#endif
