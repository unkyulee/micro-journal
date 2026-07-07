#ifdef BOARD_PICO

#include "MassStorageRP2040.h"
#include "app/app.h"
#include <Adafruit_TinyUSB.h>
#include <FatFS.h>

namespace
{
    Adafruit_USBD_MSC msc;

    bool started = false;
    bool usbConnected = false;

    bool testUnitReady()
    {
        return started;
    }

    int32_t read10(uint32_t lba, void *buffer, uint32_t bufsize)
    {
        fatfs::LBA_t hddSects;
        fatfs::disk_ioctl(0, GET_SECTOR_COUNT, &hddSects);

        fatfs::WORD ss;
        fatfs::disk_ioctl(0, GET_SECTOR_SIZE, &ss);

        if (!started || (lba >= hddSects) || (bufsize != ss))
        {
            return -1;
        }

        fatfs::disk_read(0, (uint8_t *)buffer, lba, 1);
        return bufsize;
    }

    int32_t write10(uint32_t lba, uint8_t *buffer, uint32_t bufsize)
    {
        fatfs::LBA_t hddSects;
        fatfs::disk_ioctl(0, GET_SECTOR_COUNT, &hddSects);

        fatfs::WORD ss;
        fatfs::disk_ioctl(0, GET_SECTOR_SIZE, &ss);

        if (!started || (lba >= hddSects) || (bufsize != ss))
        {
            return -1;
        }

        fatfs::disk_write(0, buffer, lba, 1);
        return bufsize;
    }

    void flush()
    {
        fatfs::disk_ioctl(0, CTRL_SYNC, nullptr);
    }

    void unplug()
    {
        if (started)
        {
            fatfs::disk_ioctl(0, CTRL_SYNC, nullptr);
            usbConnected = false;
        }
    }

    bool startStop(uint8_t power_condition, bool start, bool load_eject)
    {
        (void)power_condition;

        // Host is ejecting or re-attaching the disk.
        if (!start && load_eject)
        {
            unplug();
        }
        else if (start && load_eject)
        {
            usbConnected = true;
        }

        return true;
    }
}

//
void ms_rp2040_setup()
{
    // Register the MSC USB interface once, at boot, before the host
    // enumerates -- capacity isn't known yet (disk_initialize() hasn't
    // run), but it gets corrected via setCapacity() once the app turns
    // the drive on in ms_rp2040_loop(). The interface itself must be
    // registered exactly once and early either way.
    msc.setID("MicroJournal", "Mass Storage", "1.0");
    msc.setReadWriteCallback(read10, write10, flush);
    msc.setReadyCallback(testUnitReady);
    msc.setStartStopCallback(startStop);
    msc.begin();
}

//
void ms_rp2040_loop()
{
    static unsigned int last = millis();
    if (millis() - last > 1000)
    {
        last = millis();

        // check if the flag is on
        JsonDocument &app = status();

        // check if it is requested to turn off
        bool massStorageRequest = app["massStorage"].as<bool>();
        // false means it is requested to turn off

        // mass storage is on
        if (massStorageRequest)
        {
            // start the device
            if (app["massStorageStarted"].as<bool>() == false)
            {
                started = true;
                usbConnected = true;

                fatfs::disk_initialize(0);

                fatfs::WORD ss;
                fatfs::disk_ioctl(0, GET_SECTOR_SIZE, &ss);

                fatfs::LBA_t sectCount;
                fatfs::disk_ioctl(0, GET_SECTOR_COUNT, &sectCount);

                msc.setCapacity((uint32_t)sectCount, (uint16_t)ss);

                //
                app["massStorageStarted"] = true;
                _log("Mass Storage begin\n");
            }

            // Check if is Ejected
            if (!usbConnected)
            {
                //
                _log("Mass Storage Ejected\n");
                started = false;
                //
                app["massStorageStarted"] = false;
                app["massStorage"] = false;

                _log("Mass Storage ejected\n");
            }
        }

        else
        {
            if (app["massStorageStarted"].as<bool>() == true)
            {
                unplug();
                started = false;

                //
                app["massStorageStarted"] = false;
                _log("Mass Storage end\n");
            }
        }
    }
}

#endif
