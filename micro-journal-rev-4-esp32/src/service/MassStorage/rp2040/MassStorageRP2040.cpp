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
        // TEMP DIAGNOSTIC: confirm whether the host ever sends this at all.
        Serial.printf("MSC startStop: power_condition=%u start=%d load_eject=%d\n", power_condition, start, load_eject);

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

// Invoked on SCSI PREVENT/ALLOW MEDIUM REMOVAL (0x1E). Windows sends this
// with prohibit_removal=0 as part of "Eject" -- observed to arrive instead
// of, or before, START_STOP_UNIT depending on the driver, and Adafruit_USBD_MSC
// doesn't wire this one up itself (its own tud_msc_scsi_cb is a separate,
// unrelated fallback for SCSI opcodes core TinyUSB doesn't already handle,
// and PREVENT_ALLOW_MEDIUM_REMOVAL is one of the ones it does), so it has to
// be picked up here via the weak callback the core stack calls directly.
extern "C" bool tud_msc_prevent_allow_medium_removal_cb(uint8_t lun, uint8_t prohibit_removal, uint8_t control)
{
    (void)lun;

    // TEMP DIAGNOSTIC: confirm whether the host ever sends this at all.
    Serial.printf("MSC preventAllowMediumRemoval: prohibit_removal=%u control=%u\n", prohibit_removal, control);

    if (!prohibit_removal)
    {
        unplug();
    }

    return true;
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
                // Release the app's own FatFS mount first -- gfs() and the
                // raw fatfs::disk_* calls below share the same underlying
                // FatFS library instance/drive. Leaving it mounted while
                // the host gets raw block-level write access underneath it
                // is exactly how a stale directory/FAT cache in the app's
                // own filesystem layer ends up clobbering whatever the
                // host just wrote, corrupting or deleting files.
                gfs()->end();

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

                // reboot the device to ensure the filesystem is cleanly unmounted and the host sees it as ejected.
                watchdog_reboot(0, 0, 3000);
                delay(3000);
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
