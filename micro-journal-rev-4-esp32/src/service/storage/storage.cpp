#include "storage.h"
#include "app/app.h"
#include "app/config/config.h"

// Callback invoked when received READ10 command.
// Copy disk's data to buffer (up to bufsize) and
// return number of copied bytes (must be multiple of block size)
int32_t msc_read_cb(uint32_t lba, void *buffer, uint32_t bufsize)
{
    (void)bufsize;
    return -1; //card.readBlock(lba, (uint8_t *)buffer) ? 512 : -1;
}

// Callback invoked when received WRITE10 command.
// Process data in buffer to disk's storage and
// return number of written bytes (must be multiple of block size)
int32_t msc_write_cb(uint32_t lba, uint8_t *buffer, uint32_t bufsize)
{
    (void)bufsize;
    return -1; //card.writeBlock(lba, buffer) ? 512 : -1;
}

// Callback invoked when WRITE10 command is completed (status received and accepted by host).
// used to flush any pending cache.
void msc_flush_cb(void)
{
    // nothing to do
}

//
void storage_setup()
{
    JsonDocument &app = app_status();
    if (app["config"]["drive"].as<bool>())
    {
        //
        app_log("Starting up mass storage device\n");

        // update the config
        app["config"]["drive"] = false;
        config_save();

    }
}

///
void storage_loop()
{
}