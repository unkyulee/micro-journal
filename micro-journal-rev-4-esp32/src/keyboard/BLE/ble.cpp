#include "ble.h"
#include "app/app.h"
#include "keyboard/keyboard.h"
#include "keyboard/Locale/locale.h"
#include "display/display.h"

//
void ble_setup(const char *adName)
{
    //
    JsonDocument &app = status();

    // When ble.address exists then try to connect to the keyboard
    if (app["config"]["ble"]["address"].is<const char *>())
    {
        const char *name = app["config"]["ble"]["name"].as<const char *>();
        const char *address = app["config"]["ble"]["address"].as<const char *>();
        //
        _log("found BLE configuration try to connect %s %s\n", name, address);

        //
        ble_init(adName);
        ble_connect(address);
    }
}

//
void ble_loop()
{
    // every 30 seconds check reconnect
    static unsigned int last = millis();
    if (millis() - last > 10000)
    {
        last = millis();

        //
        JsonDocument &app = status();

        if (!app["ble_connected"].as<bool>())
        {

            // When ble.address exists then try to connect to the keyboard
            if (app["config"]["ble"]["address"].is<const char *>())
            {
                const char *name = app["config"]["ble"]["name"].as<const char *>();
                const char *address = app["config"]["ble"]["address"].as<const char *>();
                //
                _log("[ble_loop] try to connect %s %s\n", name, address);

                //
                ble_connect(address);
            }
        }
    }
}

//
static BLEClient *pClient = NULL;
BLEUUID serviceUUID = BLEUUID("1812");
static BLEUUID charUUID = BLEUUID("2A4D");
uint8_t previousKeys[8] = {};
static BLERemoteCharacteristic *pRemoteCharacteristic;
static BLERemoteService *pRemoteService;

// Initialize BLE Device
bool ble_init_done = false;
void ble_init(const char *name)
{
    if (!ble_init_done)
    {
        _log("[ble_init] Device Init: %s\n", name);

        // Start the BLEDevice
        BLEDevice::init(name);
        // Enable bonding
        BLEDevice::setSecurityAuth(true, false, true);

        //
        ble_init_done = true;
    }
}

// Custom callback class for BLE client
class clientCallback : public BLEClientCallbacks
{
    void onConnect(BLEClient *pClient)
    {
        _log("[BLEClientCallbacks] onConnect\n");
        JsonDocument &app = status();
        app["ble_connected"] = true;
    }

    void onDisconnect(BLEClient *pClient)
    {
        _log("[BLEClientCallbacks] onDisconnect\n");
        JsonDocument &app = status();
        app["ble_connected"] = false;
    }
};

static clientCallback clientCB;
uint8_t dataPrev[8];

// Callback function for notifications
void notifyCallback(
    NimBLERemoteCharacteristic *pBLERemoteCharacteristic,
    uint8_t *pData,
    size_t length,
    bool isNotify)
{
    //
    _debug("[notifyCallback] %d %d %d %d %d %d %d %d\n",
           pData[0], pData[1], pData[2], pData[3], pData[4], pData[5], pData[6], pData[7]);

    // Key Pressed
    for (int i = 2; i < 8; i++)
    {
        if (pData[i] != 0)
        {
            // check if the same key appear in the previous report
            bool newkey = true;
            for (int j = 2; j < 8; j++)
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
                _log("Key Pressed: %d %d\n", pData[i], pData[0]);
            }
        }
    }

    // Key Released
    for (int i = 2; i < 8; i++)
    {
        if (pData[i] != 0)
        {
            bool key_released = true;
            for (int j = 2; j < 8; j++)
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
                _log("Key Release: %d %d\n", dataPrev[i], dataPrev[0]);
            }
        }
    }

    //
    memcpy(dataPrev, pData, length > 8 ? 8 : length);
}

// Connect to BLE device
bool ble_connect(const char *address)
{
    _log("[ble_connect] Connecting Bluetooth Keyboard: %s\n", address);

    //
    BLEAddress *device = new BLEAddress(address);

    if (pClient && pClient->isConnected())
    {
        _log("[ble_connect] Already connected, disconnecting...\n");
        pClient->disconnect();
        NimBLEDevice::deleteClient(pClient);
        pClient = nullptr;
    }

    pClient = NimBLEDevice::createClient();
    pClient->setClientCallbacks(&clientCB);
    pClient->setConnectionParams(12, 12, 0, 51);
    pClient->setConnectTimeout(2);

    if (!pClient->connect(device))
    {
        _log("[ble_connect] Failed to connect\n");
        return false;
    }

    if (!pClient->isConnected()) {
        _log("[ble_connect] Still not connected after connect()\n");
        return false;
    }

    //
    _log("[ble_connect] Connected!\n");

    //
    pClient->getServices(true);
    pRemoteService = pClient->getService(serviceUUID);
    if (pRemoteService == nullptr)
    {
        _log("[ble_connect] cannot connect to device keyboard service ");
        pClient->disconnect();
        return false;
    }
    auto characteristicsMap = pRemoteService->getCharacteristics(true);
    if (characteristicsMap->empty())
    {
        _log("[ble_connect] Remote Service has no characteristics\n");
        pClient->disconnect();
        return false;
    }

    for (auto &characteristic : *characteristicsMap)
    {
        // if (characteristic->getUUID().toString().c_str() == charUUID.toString().c_str())
        if (characteristic->getUUID() == charUUID)
        {
            charUUID = characteristic->getUUID();
            break;
        }
    }

    pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);
    if (pRemoteCharacteristic == nullptr)
    {
        _log("[ble_connect] Failed to get characteristic from remote service: %s\n", charUUID.toString().c_str());
        pClient->disconnect();
        return false;
    }
    // Read the value of the characteristic.
    if (pRemoteCharacteristic->canRead())
    {
        pRemoteCharacteristic->readValue();
    }

    if (pRemoteCharacteristic->canNotify())
    {
        bool notify = pRemoteCharacteristic->subscribe(true, notifyCallback);
        if (!notify)
        {
            _log("[ble_connect] failed to subscribe");
        }
    }
    return true;
}
