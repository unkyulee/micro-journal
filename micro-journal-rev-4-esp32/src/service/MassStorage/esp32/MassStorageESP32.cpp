#ifdef BOARD_ESP32_S3

#include "app/app.h"
#include "MassStorageESP32.h"

USBMSC FatFSUSBClass::msc;
wl_handle_t FatFSUSBClass::wlHandle = WL_INVALID_HANDLE;

bool FatFSUSBClass::usbStarted = false;
bool FatFSUSBClass::mediaStarted = false;
bool FatFSUSBClass::ejected = false;

uint32_t FatFSUSBClass::blockCount = 0;

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
            
            if (FatFSUSB.begin())
            {
                app["massStorageStarted"] = true;
                _log("FatFSUSB begin\n");
            }
            else
            {
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

            _log("FatFSUSB ejected\n");
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

    blockCount = wl_size(wlHandle) / BLOCK_SIZE;

    if (blockCount == 0)
    {
        Serial.println("MassStorageESP32: invalid block count");

        wl_unmount(wlHandle);
        wlHandle = WL_INVALID_HANDLE;

        return false;
    }

    msc.vendorID("ESP32S3");
    msc.productID("FlashDisk");
    msc.productRevision("1.0");

    msc.onRead(readCallback);
    msc.onWrite(writeCallback);
    msc.onStartStop(startStopCallback);

    msc.mediaPresent(true);

    if (!msc.begin(blockCount, BLOCK_SIZE))
    {
        Serial.println("MassStorageESP32: MSC begin failed");

        wl_unmount(wlHandle);
        wlHandle = WL_INVALID_HANDLE;

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
        BLOCK_SIZE);

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

    mediaStarted = false;
    ejected = false;
    blockCount = 0;

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

    const size_t address = static_cast<size_t>(lba) * BLOCK_SIZE + offset;

    esp_err_t err = wl_read(wlHandle, address, buffer, bufsize);

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

    const size_t address = static_cast<size_t>(lba) * BLOCK_SIZE + offset;

    esp_err_t err = wl_write(wlHandle, address, buffer, bufsize);

    if (err != ESP_OK)
    {
        return -1;
    }

    return static_cast<int32_t>(bufsize);
}

bool FatFSUSBClass::startStopCallback(
    uint8_t power_condition,
    bool start,
    bool load_eject)
{
    (void)power_condition;

    if (load_eject && !start)
    {
        ejected = true;
        mediaStarted = false;
    }

    return true;
}

#endif