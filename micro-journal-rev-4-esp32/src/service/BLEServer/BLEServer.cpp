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
                const char *address = advertisedDevice->getAddress().toString().c_str();
                const char *name = advertisedDevice->getName().c_str();

                // Add to the config
                app["ble_devices"][i]["address"] = address;
                app["ble_devices"][i]["name"] = name;

                //
                _log("BLE device found index: %d name: %s address: %s\n", i, name, address);
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
    if (millis() - last > 1000)
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
            app["ble_state"] = BLE_SCAN_STARTED;
            app["ble_error"] = "";
            app["ble_message"] = "Scanning BLE Devices";
            app["ble_devices"] = app["ble"]["devices"].to<JsonArray>();
            app["clear"] = true;

            // Scan BLE devices
            pBLEScan = BLEDevice::getScan();
            pBLEScan->clearResults();
            pBLEScan->setAdvertisedDeviceCallbacks(new advertisedDeviceCallback());
            pBLEScan->start(5, false);
            delay(5000);

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
    }
}

// Entry Screen Initializing
void BLEServer_setup()
{
    //
    buffer_clear();

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

    /*




    //


    //
    if (wifi_config_status >= WIFI_CONFIG_EDIT_SSID)
    {
        // SAVE or NEXT
        if (key == '\n')
        {
            // NEXT step
            if (wifi_config_status == WIFI_CONFIG_EDIT_SSID)
            {
                // save ssid
                JsonArray savedAccessPoints = app["wifi"]["access_points"].as<JsonArray>();
                savedAccessPoints[wifi_config_index]["ssid"] = String(buffer_get());

                // clear buffer
                buffer_clear();

                // move to password enter screen
                wifi_config_status = WIFI_CONFIG_EDIT_KEY;
                app["wifi_config_status"] = wifi_config_status;
            }
            else if (wifi_config_status == WIFI_CONFIG_EDIT_KEY)
            {
                // save ssid
                JsonArray savedAccessPoints = app["wifi"]["access_points"].as<JsonArray>();
                savedAccessPoints[wifi_config_index]["password"] = String(buffer_get());

                // save the configuration
                wifi_config_save();

                //
                buffer_clear();

                // go back to the configuration list
                wifi_config_status = WIFI_CONFIG_LIST;
                app["wifi_config_status"] = wifi_config_status;
            }
        }
        // BACK SPACE
        else if (key == '\b')
        {
            // backspace
            buffer_remove();
        }
        // ADD KEYS
        else
        {
            // edit mode
            buffer_add(key);

            //
            _debug("BLEServer_keyboard buffer_add: %c, %s\n", key, buffer_get());
        }
    }
    else
    {
        // back to home
        if (key == 'B' || key == 'b')
        {
            //
            // go back to home menu
            app["menu"]["state"] = MENU_HOME;

            return;
        }

        else if (key >= '1' && key <= '5')
        {
            //
            // wifi entry chose to edit
            //

            // clear buffer
            buffer_clear();

            // determine the index to edit
            wifi_config_index = key - '1';
            _log("Wifi Entry chosen %d\n", wifi_config_index);
            app["wifi_config_index"] = wifi_config_index;

            // move the screen
            wifi_config_status = WIFI_CONFIG_EDIT_SSID;
            app["wifi_config_status"] = wifi_config_status;

            return;
        }
    }



        */

    /*
JsonDocument &app = status();

if (PairBLE_config_status == PairBLE_CONFIG_POST_SCAN)
{
    // choose the device
    if (key == 'm' || key == 'M' || key == MENU)
    {
        // check if it is back
        if (deviceSelected >= deviceIndex)
        {
            // go back
            PairBLE_config_status = PairBLE_CONFIG_LIST;
            return;
        }
        else
        {
            // picked a device
            _log("Picked %d, %s\n", key, foundDevices[deviceSelected].name.c_str());
            JsonObject ble = app["config"]["ble"].as<JsonObject>();
            ble["remote"] = foundDevices[deviceSelected].address;
            PairBLE_save();
        }
    }
    else if (key == 'B' || key == 'b')
    {
        if (deviceIndex == 0)
        {
            // back to
            PairBLE_config_status = PairBLE_CONFIG_LIST;
        }
        else
        {
            deviceSelected++;
            if (deviceSelected > deviceIndex)
                deviceSelected = 0;
        }

        return;
    }

    // reset
    for (int i = 0; i < deviceIndex; i++)
    {
        foundDevices[deviceIndex] = device();
    }
    deviceIndex = 0;
    PairBLE_config_status = PairBLE_CONFIG_LIST;

    return;
}
else if (PairBLE_config_status == PairBLE_CONFIG_EDIT_ADDR)
{
    // BACK SPACE
    if (key == '\b')
    {
        // backspace
        PairBLE_config_buffer[PairBLE_config_buffer_pos--] = '\0';
    }
    else if (key == '\n')
    {
        // save address
        JsonObject ble = app["config"]["ble"].as<JsonObject>();
        ble["remote"] = String(PairBLE_config_buffer);

        PairBLE_config_buffer[0] = '\0';
        PairBLE_config_buffer_pos = 0;

        PairBLE_save();

        //
        PairBLE_config_status = PairBLE_CONFIG_LIST;
    }
    else
    {
        // edit mode
        PairBLE_config_buffer[PairBLE_config_buffer_pos++] = key;
        PairBLE_config_buffer[PairBLE_config_buffer_pos] = '\0';
    }

    // OVERFLOW CONTROL
    if (PairBLE_config_buffer_pos > PairBLE_CONFIG_BUFFER)
    {
        PairBLE_config_buffer[0] = '\0';
        PairBLE_config_buffer_pos = 0;
    }
}
else
{
    // back to home
    if (key == 'B' || key == 'b')
    {
        //
        // go back to home menu
        app["menu"]["state"] = MENU_HOME;

        return;
    }

    else if (key == 'E' || key == 'e')
    {
        // remove the BLE config
        app["config"]["ble"]["remote"] = "";
        config_save();


        // bluetooth entry chose to edit
        //PairBLE_config_status = PairBLE_CONFIG_EDIT_ADDR;
        //PairBLE_config_buffer[0] = '\0';
        //PairBLE_config_buffer_pos = 0;


        //
        Menu_clear();

        return;
    }
    else if (key == 'T' || key == 't')
    {
        //
        // Toggle scan mode
        app["config"]["ble"]["scan_mode"] = !app["config"]["ble"]["scan_mode"];
        PairBLE_save();
        return;
    }
    else if ((key == 'M' || key == 'm' || key == MENU) && !keyboard_ble_connected())
    {
        PairBLE_config_status = PairBLE_CONFIG_SCANNING;

        return;
    }
}
*/
}
