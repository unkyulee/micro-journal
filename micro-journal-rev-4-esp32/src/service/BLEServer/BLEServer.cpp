#include "BLEServer.h"
#include "app/app.h"
#include "display/display.h"
#include "keyboard/BLE/ble.h"

// services
#include "service/Buffer/BufferService.h"

// HID service UUID
static BLEUUID hidUUID((uint16_t)0x1812);

/////////////////////////////////////////////////////
// BACKGROUND SERVICE
/////////////////////////////////////////////////////
void BLEServer_init()
{
    // load app status
    JsonDocument &app = status();

    // reset task queue
    app["ble_state"] = 0;
    app["ble_error"] = "";
    app["ble_message"] = "";
}

void BLEServer_loop()
{
    static unsigned int last = 0;
    if (millis() - last > 10000)
    {
        last = millis();

        //
        JsonDocument &app = status();
        String task = app["task"].as<String>();

        // scan for the BLE devices
        if (task == "ble_scan")
        {
            //
            app["task"] = "";

            //
            _log("[BLEServer_loop] Picked up BLE Device Scan Request\n");

            //
            BLEServer_scan();

            // Check if any keyboard is found
            JsonArray ble_devices = app["ble_devices"].as<JsonArray>();
            if (ble_devices.size() == 0)
            {
                _log("No BLE Devices Found\n");
                app["ble_state"] = BLE_CONFIG_NO_DEVICES;
            }
        }

        else if (task == "ble_connect")
        {
            //
            app["task"] = "";

            //
            const char *name = app["config"]["ble"]["name"].as<const char *>();
            const char *address = app["config"]["ble"]["address"].as<const char *>();
            const int type = app["config"]["ble"]["type"].as<int>();
            _log("[BLEServer_loop] Picked up BLE Device Connect Request: %s\n", name);

            // try to connect directly
            //
            if (!ble_connect(address, type))
            {
                //
                BLEServer_scan();

                // Check if any keyboard is found
                JsonArray devices = app["ble_devices"].as<JsonArray>();

                // See if any devices matches
                _log("See if paired device found: %d\n", devices.size());

                //
                for (int i = 0; i < devices.size(); i++)
                {
                    const char *targetName = devices[i]["name"].as<const char *>();
                    const char *targetAddress = devices[i]["address"].as<const char *>();
                    int targetAddressType = devices[i]["type"].as<int>();
                    if (strcmp(name, targetName) == 0)
                    {
                        // found the device
                        _log("[BLEServer_loop] connecting to %s %s %d\n", targetName, targetAddress, targetAddressType);
                        //
                        ble_connect(targetAddress, targetAddressType);
                    }
                }
            }
        }
    }
}

void BLEServer_scan()
{
    _log("BLE Devices Scan Starting\n");

    //
    JsonDocument &app = status();
    //
    app["ble_error"] = "";
    app["ble_message"] = "Scanning BLE Devices";
    app["ble_devices"] = app["ble_devices"].to<JsonArray>();
    app["clear"] = true;

    // Scan BLE devices
    NimBLEScan *pScan = NimBLEDevice::getScan();
    NimBLEScanResults results = pScan->getResults(5 * 1000);
    JsonArray ble_devices = app["ble_devices"].as<JsonArray>();
    ble_devices.clear();

    int count = 0;
    for (int i = 0; i < results.getCount(); i++)
    {
        const NimBLEAdvertisedDevice *device = results.getDevice(i);

        if (device->isAdvertisingService(hidUUID))
        {
            // retrieve information
            // Add to the config
            app["ble_devices"][count]["address"] = device->getAddress().toString().c_str();
            app["ble_devices"][count]["name"] = device->getName().c_str();
            app["ble_devices"][count]["type"] = (int)device->getAddressType();

            //
            _log("BLE device found index: %d name: %s address: %s type: %d\n",
                 i,
                 app["ble_devices"][count]["name"].as<const char *>(),
                 app["ble_devices"][count]["address"].as<const char *>(),
                 app["ble_devices"][count]["type"].as<int>());

            //
            count++;
        }
    }

    // refresh the menu screen
    _log("BLE Devices Scan Ended\n");
    app["clear"] = true;
}

// Entry Screen Initializing
void BLEServer_setup(const char *name)
{
    //
    buffer_clear();

    // init BLE device
    ble_init(name);

    //
    JsonDocument &app = status();

    // initialize the config
    if (!app["ble"].is<JsonObject>())
    {
        JsonObject ble = app["ble"].to<JsonObject>();
        app["ble"] = ble;
    }

    if (!app["ble"]["devices"].is<JsonArray>())
    {
        JsonArray devices = app["ble"]["devices"].to<JsonArray>();
        app["ble"]["devices"] = devices;
    }

    // Request for BLE Devices Scan
    app["task"] = "ble_scan";
    app["ble_state"] = BLE_CONFIG_LIST;
}

//
void BLEServer_keyboard(char key)
{
    // non printable keys are not going to be going through the buffer
    if (key == 0)
        return;

    //
    JsonDocument &app = status();

    //
    _debug("BLEServer_keyboard key: %d\n", key);

    // back to home
    if (key == 'B' || key == 'b')
    {
        app["menu"]["state"] = MENU_HOME;
        return;
    }
}
