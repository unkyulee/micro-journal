#include "ble.h"
#include "app/app.h"
#include "keyboard/keyboard.h"
#include "keyboard/Locale/locale.h"
#include "display/display.h"

#include "service/BLEServer/BLEServer.h"

//
void ble_setup(const char *adName)
{
    //
    JsonDocument &app = status();

    // When ble.address exists then try to connect to the keyboard
    if (app["config"]["ble"]["address"].is<const char *>())
    {
        // BLE Initialize
        ble_init(adName);

        // try to connect directly
        const char *address = app["config"]["ble"]["address"].as<const char *>();
        const int type = app["config"]["ble"]["type"].as<int>();

        //
        if (!ble_connect(address, type))
        {
            // Initiate scan
            app["task"] = "ble_connect";
        }
    }
}

//
void ble_loop()
{
    //
    JsonDocument &app = status();

    // nothing to do when BLE is not paired
    if (!app["config"]["ble"]["address"].is<const char *>())
        return;

    // every 10 seconds check reconnect
    static unsigned int last = millis();
    if (millis() - last > 10000)
    {
        last = millis();

        if (!app["ble_connected"].as<bool>())
        {
            // When ble.address exists then try to connect to the keyboard
            if (app["config"]["ble"]["address"].is<const char *>())
            {
                // Initiate scan
                if (app["task"].as<String>().isEmpty())
                    app["task"] = "ble_connect";
            }
        }
    }
}

// Initialize BLE Device
bool ble_init_done = false;
void ble_init(const char *name)
{
    if (!ble_init_done)
    {
        _log("[ble_init] Device Init: %s\n", name);

        // Start the BLEDevice
        NimBLEDevice::init(name);

        //
        ble_init_done = true;
    }
}

// Custom callback class for BLE client
class clientCallback : public NimBLEClientCallbacks
{
    void onConnect(NimBLEClient *pClient)
    {
        _log("[BLEClientCallbacks] onConnect\n");
        JsonDocument &app = status();
        app["ble_connected"] = true;
    }

    void onConnectFail(NimBLEClient *pClient, int reason)
    {
        _log("[BLEClientCallbacks] onConnectFail %d\n", reason);
        JsonDocument &app = status();
        app["ble_connected"] = false;
    }

    void onDisconnect(NimBLEClient *pClient, int reason)
    {
        _log("[BLEClientCallbacks] onDisconnect %d\n", reason);
        JsonDocument &app = status();
        app["ble_connected"] = false;
    }
};

static clientCallback clientCB;
uint8_t dataPrev[7];

// Callback function for notifications
void notifyCallback(
    NimBLERemoteCharacteristic *pBLERemoteCharacteristic,
    uint8_t *pData,
    size_t length,
    bool isNotify)
{
    //
    _debug("[notifyCallback] size [%d] %d %d %d %d %d %d %d\n",
           length, pData[0], pData[1], pData[2], pData[3], pData[4], pData[5], pData[6]);
    _debug("[notifyCallback] prev [%d] %d %d %d %d %d %d %d\n",
           length, dataPrev[0], dataPrev[1], dataPrev[2], dataPrev[3], dataPrev[4], dataPrev[5], dataPrev[6]);

    // Key Pressed
    for (int i = 1; i < length; i++)
    {
        if (pData[i] != 0)
        {
            // check if the same key appear in the previous report
            bool newkey = true;
            for (int j = 1; j < length; j++)
            {
                if (dataPrev[j] == pData[i])
                {
                    newkey = false;
                    break;
                }
            }

            // otherwise register new key press
            if (newkey)
            {
                // handle key pressed
                keyboard_HID2Ascii(pData[i], pData[0], true);
                _debug("Key Pressed: %d %d\n", pData[i], pData[0]);
            }
        }
    }

    // Key Released
    for (int i = 1; i < length; i++)
    {
        if (dataPrev[i] != 0)
        {
            bool key_released = true;
            for (int j = 1; j < length; j++)
            {
                if (dataPrev[i] == pData[j])
                {
                    key_released = false;
                    break;
                }
            }
            if (key_released)
            {
                keyboard_HID2Ascii(dataPrev[i], dataPrev[0], false);
                _debug("Key Release: %d %d\n", dataPrev[i], dataPrev[0]);
            }
        }
    }

    //
    memcpy(dataPrev, pData, length > 7 ? 7 : length);
}

// Connect to BLE device
NimBLEUUID serviceUUID = NimBLEUUID("1812");
NimBLEUUID charUUID = NimBLEUUID("2A4D");
NimBLEClient *client;
NimBLERemoteService *service;
NimBLERemoteCharacteristic *characteristic;

bool ble_connect(const char *address, int addrType)
{
    _log("[ble_connect] address %s\n", address);

    // Client Setup
    client = NimBLEDevice::createClient();
    if (!client)
    {
        _log("[ble_connect] Failed to create client\n");
        return false;
    }
    client->setClientCallbacks(&clientCB, false);
    client->setConnectionParams(12, 12, 0, 51);
    client->setConnectTimeout(2000);

    // Address Setup
    NimBLEAddress bleAddr = NimBLEAddress(std::string(address), addrType);

    // try to connect
    if (!client->connect(bleAddr))
    {
        _log("[ble_connect] Failed to connect\n");
        client->setClientCallbacks(nullptr);
        NimBLEDevice::deleteClient(client);
        client = nullptr;
        return false;
    }

    // check if connected
    if (!client->isConnected())
    {
        _log("[ble_connect] Still not connected after connect()\n");
        client->setClientCallbacks(nullptr);
        NimBLEDevice::deleteClient(client);
        client = nullptr;
        return false;
    }

    _log("[ble_connect] Connected!\n");

    // Try to setup the Keyboard
    if (!client->discoverAttributes())
    {
        _log("[ble_connect] Failed to discover attributes\n");
        client->disconnect();
        client->setClientCallbacks(nullptr);
        NimBLEDevice::deleteClient(client);
        client = nullptr;
        return false;
    }

    service = client->getService(serviceUUID);
    if (service == nullptr)
    {
        _log("[ble_connect] Cannot find service %s\n", serviceUUID.toString().c_str());
        client->disconnect();
        client->setClientCallbacks(nullptr);
        NimBLEDevice::deleteClient(client);
        client = nullptr;
        return false;
    };

    characteristic = service->getCharacteristic(charUUID);
    if (characteristic == nullptr)
    {
        _log("[ble_connect] Failed to get characteristic %s\n", charUUID.toString().c_str());
        client->disconnect();
        client->setClientCallbacks(nullptr);
        NimBLEDevice::deleteClient(client);
        client = nullptr;
        return false;
    }

    if (characteristic->canRead())
    {
        std::string val = characteristic->readValue();
        _log("[ble_connect] Read value size: %d\n", (int)val.size());
    }

    if (characteristic->canNotify())
    {
        bool notify = characteristic->subscribe(true, notifyCallback);
        if (!notify)
        {
            _log("[ble_connect] failed to subscribe");
            client->disconnect();
            client->setClientCallbacks(nullptr);
            NimBLEDevice::deleteClient(client);
            client = nullptr;
            return false;
        }
    }

    return true;
}
