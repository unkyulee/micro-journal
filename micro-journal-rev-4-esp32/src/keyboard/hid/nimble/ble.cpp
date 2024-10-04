#include "ble.h"
#include "app/app.h"
#include "keyboard/keyboard.h"

static BLEClient *pClient = nullptr;
static BLEUUID charUUID = BLEUUID("2A4D");
static BLERemoteCharacteristic *pRemoteCharacteristic;
static BLERemoteService *pRemoteService;
static bool scanMode = false;
uint8_t previousKeys[8] = {};

//
BLEUUID serviceUUID = BLEUUID("1812");
BLEUUID keyboard_ble_service_uuid()
{
    return serviceUUID;
}

//
bool ble_connected = false;
bool keyboard_ble_connected()
{
    return ble_connected;
}
void keyboard_ble_connected_set(bool connected)
{
    ble_connected = connected;
}

bool keyboard_ble_connect()
{
    JsonDocument &app = app_status();

    // look for BLE client
    JsonString remote = app["config"]["ble"]["remote"].as<JsonString>();
    if (remote == "null" || remote.isNull() || remote == "")
    {
        return false;
    }

    // initialize BLE only when it configured
    BLEDevice::init("uJournal");
    // Enable bonding
    BLEDevice::setSecurityAuth(true, false, true);

    //
    scanMode = app["config"]["ble"]["scan_mode"].as<bool>();
    if (scanMode)
    {
        app_log("scan mode true, skipping connect\n");
        return false;
    }

    app_log("Connecting Bluetooth Keyboard: %s\n", remote);
    const char *address = (const char *)remote.c_str();
    connectToServer(address, BLE_ADDR_RANDOM);

    return true;
}

void keyboard_ble_loop()
{
    // if (!ble_connected)
    // {
    //     keyboard_ble_connect();
    // }
}

bool keyboard_ble_setup()
{
    app_log("Init BLE Keyboard\n");

    return keyboard_ble_connect();
}

// Callback function for notifications
void notifyCallback(
    NimBLERemoteCharacteristic *pBLERemoteCharacteristic,
    uint8_t *pData,
    size_t length,
    bool isNotify)
{
    // pData[2-7] are active keycodes, diff against previous keys to find
    // keys that have been pressed to trigger this notification
    // init to first key
    uint8_t keycode = pData[2];
    bool changed = false;
    for (uint8_t i = 2; i < length; i++)
    {
        if (previousKeys[i] != pData[i])
        {
            keycode = pData[i];
            changed = true;
        }
        previousKeys[i] = pData[i];
    }

    // this callback can be triggered if the modifier is
    // released before the alpha - if pData[2-7] haven't
    // changed then there's nothing to do.
    if (!changed)
    {
        return;
    }

    uint8_t shift = (pData[0] == 0x2 || pData[0] == 0x20);
    uint8_t altgr = (pData[0] == 0x4 || pData[0] == 0x40);

    /* TBD
    uint8_t ascii = getKeycodeToAscii(keycode, shift, altgr, false);
    onKeyboardKey(ascii, keycode, shift);
    */
}

// Custom callback class for BLE client
class clientCallback : public BLEClientCallbacks
{
    void onConnect(BLEClient *pClient)
    {
        app_log("connect callback");
        ble_connected = true;
    }

    void onDisconnect(BLEClient *pClient)
    {
        app_log("onDisconnect");
        ble_connected = false;
    }
};

static clientCallback clientCB;

bool connectToServer(const char *remote, uint8_t type)
{
    BLEAddress *device = new BLEAddress(remote, type);

    // Connect to the BLE Server that has the name, Service, and Characteristics
    if (NimBLEDevice::getClientListSize())
    {
        app_log("client list size: %d\n", NimBLEDevice::getClientListSize());
        pClient = NimBLEDevice::getClientByPeerAddress(*device);
        if (pClient)
        {
            if (!pClient->connect(device))
            {
                app_log("Reconnect failed\n");
                return false;
            }
        }
    }
    else
    {
        pClient = NimBLEDevice::createClient(NimBLEAddress(device->toString(), BLE_ADDR_RANDOM));
    }

    pClient->setClientCallbacks(&clientCB, false);
    pClient->setConnectionParams(12, 12, 0, 51);
    pClient->setConnectTimeout(2);
    if (!pClient->connect(new NimBLEAddress(device->toString(), type)))
    {
        app_log("BLE: could not connect: %s\n", device->toString().c_str());
        return false;
    };
    if (!pClient->isConnected())
    {
        app_log("BLE: connect returned true but not connected\n");
        return false;
    }
    app_log("BLE: Connected to server\n");

    pClient->getServices(true);
    pRemoteService = pClient->getService(serviceUUID);
    if (pRemoteService == nullptr)
    {
        app_log("BLE: cannot connect to device keyboard service ");
        pClient->disconnect();
        return false;
    }
    auto characteristicsMap = pRemoteService->getCharacteristics(true);
    if (characteristicsMap->empty())
    {
        app_log("BLE: Remote Service has no characteristics\n");
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
        app_log("BLE: Failed to get characteristic from remote service: %s\n", charUUID.toString().c_str());
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
            app_log("BLE: failed to subscribe");
        }
    }
    return true;
}
