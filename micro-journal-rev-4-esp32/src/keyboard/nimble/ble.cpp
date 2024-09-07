#include "ble.h"
#include "app/app.h"
#include "keyboard/usb/button/button.h"
#include "keyboard/usb/usb.h" // for keyboard_layout
#include "keyboard/keyboard.h"

static BLEClient *pClient = nullptr;
static BLEUUID charUUID = BLEUUID("2A4D");
static BLERemoteCharacteristic *pRemoteCharacteristic;
static BLERemoteService *pRemoteService;
static bool scanMode = false;
uint8_t previousKeys[8] = {};

bool keyboard_ble_connect()
{
    JsonDocument &app = app_status();

    // look for BLE client
    JsonString remote = app["config"]["ble"]["remote"].as<JsonString>();
    if (remote == "null" || remote.isNull())
    {
        return false;
    }
    scanMode = app["config"]["ble"]["scan_mode"].as<bool>();
    if (scanMode)
    {
        app_log("scan mode true, skipping connect\n");
        return false;
    }

    const char* address = (const char*)remote.c_str();
    connectToServer(address, BLE_ADDR_RANDOM);

    // update the locale depending on your keyboard layout
    //
    String layout = app["config"]["keyboard_layout"].as<String>();
    if (layout == "null" || layout.isEmpty())
        layout = "US"; // defaults to US layout

    keyboard_layout(layout);

    // setup display button
    button_setup();

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
    BLEDevice::init("uJournal");
    // Enable bonding
    BLEDevice::setSecurityAuth(true, false, true);

    return keyboard_ble_connect();
}

// Callback function for notifications
void notifyCallback(
    NimBLERemoteCharacteristic* pBLERemoteCharacteristic,
    uint8_t* pData,
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
    uint8_t ascii = getKeycodeToAscii(keycode, shift, altgr);
    onKeyboardKey(ascii, keycode, shift);
}

// Custom callback class for BLE client
class clientCallback : public BLEClientCallbacks
{
    void onConnect(BLEClient* pClient) {
        app_log("connect callback");
        ble_connected = true;
    }

    void onDisconnect(BLEClient* pClient) {
        app_log("onDisconnect");
        ble_connected = false;
    }
};

static clientCallback clientCB;

bool connectToServer(const char* remote, uint8_t type)
{
    BLEAddress *device = new BLEAddress(remote, type);

    //Connect to the BLE Server that has the name, Service, and Characteristics
    if(NimBLEDevice::getClientListSize()) {
        app_log("client list size: %d\n", NimBLEDevice::getClientListSize());
        pClient = NimBLEDevice::getClientByPeerAddress(*device);
        if(pClient)
        {
            if(!pClient->connect(device)) {
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
    pClient->setConnectionParams(12,12,0,51);
    pClient->setConnectTimeout(2);
    if (!pClient->connect(new NimBLEAddress(device->toString(), type))) {
        app_log("BLE: could not connect\n");
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

    for (auto& characteristic : *characteristicsMap)
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
        if (!notify) {
            app_log("BLE: failed to subscribe");
        }
    }
    return true;
}

