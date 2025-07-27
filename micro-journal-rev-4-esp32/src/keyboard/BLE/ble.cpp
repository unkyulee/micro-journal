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

// Callback function for notifications
void notifyCallback(
    NimBLERemoteCharacteristic *pBLERemoteCharacteristic,
    uint8_t *pData,
    size_t length,
    bool isNotify)
{
    //
    _debug("[notifyCallback] %d %d %d %d %d %d\n", 
        pData[0], pData[1], pData[2], pData[3], pData[4], pData[5]);

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

    JsonDocument &app = status();
    String locale = app["config"]["keyboard_layout"].as<String>();
    uint8_t ascii = keyboard_keycode_ascii(locale, keycode, shift, altgr);
    if (ascii != 0)
    {
        // send key to GUI
        display_keyboard(ascii, true, keycode);
        display_keyboard(ascii, false, keycode);
    }

    //
    _log("BLE Keyboard Pressed keycode: %d ascii: %d\n", keycode, ascii);
}

// Connect to BLE device
bool ble_connect(const char *address)
{
    _log("[ble_connect] Connecting Bluetooth Keyboard: %s\n", address);

    //
    BLEAddress *device = new BLEAddress(address, BLE_ADDR_RANDOM);

    // Connect to the BLE Server that has the name, Service, and Characteristics
    if (NimBLEDevice::getClientListSize())
    {
        _log("[ble_connect] client list size: %d\n", NimBLEDevice::getClientListSize());
        pClient = NimBLEDevice::getClientByPeerAddress(*device);
        if (pClient)
        {
            if (!pClient->connect(device))
            {
                _log("[ble_connect] Reconnect failed\n");
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
    if (!pClient->connect(new NimBLEAddress(device->toString(), BLE_ADDR_RANDOM)))
    {
        _log("[ble_connect] could not connect: %s\n", device->toString().c_str());
        return false;
    };
    if (!pClient->isConnected())
    {
        _log("[ble_connect] connect returned true but not connected\n");
        return false;
    }
    _log("[ble_connect] Connected to server\n");

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
