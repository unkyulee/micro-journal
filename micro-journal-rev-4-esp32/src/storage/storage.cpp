#include "storage.h"
#include "app/app.h"
#include "SD/sd.h"
#include "config/config.h"
#include "display/display.h"

//
#include <SD.h>
#include "USB.h"
#include "USBMSC.h"

static const uint32_t DISK_SECTOR_COUNT = 240 * 1000; // 8KB is the smallest size that windows allow to mount
static const uint16_t DISK_SECTOR_SIZE = 512;         // Should be 512
static const uint16_t DISC_SECTORS_PER_TABLE = 1;     // each table sector can fit 170KB (340 sectors)

USBMSC usb_msc; // USB Mass Storage object

// Set to true when PC write to flash
bool sd_changed = false;
bool sd_inited = false;

static int32_t onWrite(uint32_t lba, uint32_t offset, uint8_t *buffer, uint32_t bufsize)
{
    //Serial.printf("write lba %d offset %d size %d\n", lba, offset, bufsize);
    return SD.writeRAW((uint8_t *)buffer, lba) ? bufsize/512 : -1;
}

static int32_t onRead(uint32_t lba, uint32_t offset, void *buffer, uint32_t bufsize)
{
    //Serial.printf("read lba %d offset %d size %d\n", lba, offset, bufsize);
    return SD.readRAW((uint8_t *)buffer, lba) ? bufsize/512 : -1;
}

static bool onStartStop(uint8_t power_condition, bool start, bool load_eject)
{
    Serial.println("");
    Serial.printf("MSC START/STOP: power: %u, start: %u, eject: %u\n", power_condition, start, load_eject);
    return true;
}

static void usbEventCallback(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    if (event_base == ARDUINO_USB_EVENTS)
    {
        arduino_usb_event_data_t *data = (arduino_usb_event_data_t *)event_data;
        switch (event_id)
        {
        case ARDUINO_USB_STARTED_EVENT:
            Serial.println("");
            Serial.println("USB PLUGGED");
            break;
        case ARDUINO_USB_STOPPED_EVENT:
            Serial.println("");
            Serial.println("USB UNPLUGGED");
            break;
        case ARDUINO_USB_SUSPEND_EVENT:
            Serial.println("");
            Serial.printf("USB SUSPENDED: remote_wakeup_en: %u\n", data->suspend.remote_wakeup_en);
            break;
        case ARDUINO_USB_RESUME_EVENT:
            Serial.println("");
            Serial.println("USB RESUMED");
            break;

        default:
            break;
        }
    }
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

        Serial.print("card size = ");
        Serial.print(SD.cardSize());
        Serial.print(", numSectors = ");
        Serial.print(SD.numSectors());
        Serial.print(", bytes per sector = ");
        Serial.print(SD.cardSize() / SD.numSectors());
        Serial.print(", total bytes = ");
        Serial.print(SD.totalBytes());
        Serial.print(", usedBytes = ");
        Serial.print(SD.usedBytes());

        usb_msc.vendorID("Reflect");    // max 8 chars
        usb_msc.productID("MSC");       // max 16 chars
        usb_msc.productRevision("1.1"); // max 4 chars
        usb_msc.onStartStop(onStartStop);
        usb_msc.onRead(onRead);
        usb_msc.onWrite(onWrite);
        usb_msc.mediaPresent(true);
        usb_msc.begin(SD.numSectors(), SD.cardSize() / SD.numSectors());

        USB.onEvent(usbEventCallback);
        USB.begin();

        sd_changed = true; // to print contents initially

        // load usb screen
        app["screen"] = USBDRIVESCREEN;
    }
}
