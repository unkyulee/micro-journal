#include "BLEServer.h"
#include "app/app.h"
#include "display/display.h"
#include "keyboard/BLE/ble.h"

// services
#include "service/Buffer/BufferService.h"

//
static BLEScan *pBLEScan = nullptr;
class advertisedDeviceCallback : public BLEAdvertisedDeviceCallbacks
{
    void onResult(BLEAdvertisedDevice *advertisedDevice)
    {
        // HID service UUID
        static BLEUUID hidUUID((uint16_t)0x1812);

        //
        JsonDocument &app = status();
        for (int i = 0; i < advertisedDevice->getServiceUUIDCount(); i++)
        {
            //
            BLEUUID service = advertisedDevice->getServiceUUID(i);
            if (service.equals(hidUUID))
            {
                // Keyboard Device Found
                std::string address = advertisedDevice->getAddress().toString();
                std::string name = advertisedDevice->getName();

                // Add to the config
                app["ble_devices"][i]["address"] = address.c_str();
                app["ble_devices"][i]["name"] = name.c_str();
                app["ble_devices"][i]["type"] = (int)advertisedDevice->getAddressType();

                //
                _log("BLE device found index: %d name: %s address: %s type: %d\n",
                     i, name.c_str(), address.c_str(), advertisedDevice->getAddressType());
            }
        }
    }
};

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
            JsonDocument &app = status();

            //
            app["ble_error"] = "";
            app["ble_message"] = "Scanning BLE Devices";
            app["ble_devices"] = app["ble_devices"].to<JsonArray>();
            app["clear"] = true;

            // Scan BLE devices
            pBLEScan = BLEDevice::getScan();
            pBLEScan->clearResults();
            pBLEScan->setAdvertisedDeviceCallbacks(new advertisedDeviceCallback());
            pBLEScan->start(5);

            // Check if any keyboard is found
            JsonArray ble_devices = app["ble_devices"].as<JsonArray>();
            if (ble_devices.size() == 0)
            {
                _log("No BLE Devices Found\n");
                app["ble_state"] = BLE_CONFIG_NO_DEVICES;
            }

            // refresh the menu screen
            _log("BLE Devices Scan Ended\n");
            app["clear"] = true;
        }

        else if (task == "ble_connect")
        {
            //
            app["task"] = "";

            //
            const char *name = app["config"]["ble"]["name"].as<const char *>();
            _log("[BLEServer_loop] Picked up BLE Device Connect Request: %s\n", name);

            //
            // Scan BLE devices
            pBLEScan = BLEDevice::getScan();
            pBLEScan->clearResults();
            pBLEScan->setAdvertisedDeviceCallbacks(new advertisedDeviceCallback());
            pBLEScan->start(5);

            // Check if any keyboard is found
            JsonArray devices = app["ble_devices"].as<JsonArray>();
            for (int i = 0; i < devices.size(); i++)
            {
                const char *targetName = devices[i]["name"].as<const char *>();
                const char *targetAddress = devices[i]["address"].as<const char *>();
                int targetAddressType = devices[i]["type"].as<int>();
                if (strcmp(name, targetName) == 0)
                {
                    // found the device
                    _log("[BLEServer_loop] found %s %s %d\n", targetName, targetAddress, targetAddressType);
                    ble_connect(targetAddress, targetAddressType);
                }
            }
        }
    }
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