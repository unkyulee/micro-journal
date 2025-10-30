#include "ble.h"
#include "app/app.h"
#include "keyboard/keyboard.h"
#include "keyboard/Locale/locale.h"
#include "display/display.h"

#include "service/BLEServer/BLEServer.h"

// notify callback shouldn't be blocked
// any file operation triggers will crash the system
// so copy the message from the callback and process it in the main process
// Define the HID report structure
typedef struct
{
    uint8_t report[7];
} HidReport_t;

// Queue to store HID reports
static QueueHandle_t hidQueue = nullptr;

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

    // Create HID report queue (10 elements of HidReport_t)
    hidQueue = xQueueCreate(10, sizeof(HidReport_t));
    assert(hidQueue != NULL);
}

//
void ble_loop()
{
    //
    JsonDocument &app = status();

    // nothing to do when BLE is not paired
    // if (!app["config"]["ble"]["address"].is<const char *>())
    //    return;

    // every 10 seconds check reconnect
    static unsigned int last = millis();
    if (millis() - last > 10000)
    {
        last = millis();

        bool ble_connected = app["ble_connected"].as<bool>();
        _debug("BLE Keyboard Connection: %d\n", ble_connected);
        if (ble_connected == false)
        {
            // When ble.address exists then try to connect to the keyboard
            if (app["config"]["ble"]["address"].is<const char *>())
            {
                //
                _log("BLE configuration found: %s\n", app["config"]["ble"]["address"].as<const char *>());

                // Initiate scan
                if (app["task"].as<String>().isEmpty())
                    app["task"] = "ble_connect";
            }
        }
    }

    // Process HID queue safely
    HidReport_t report;
    while (hidQueue != nullptr && xQueueReceive(hidQueue, &report, 0) == pdTRUE)
    {
        static uint8_t prev[7] = {0};

        // Key Pressed
        for (int i = 1; i < 7; i++)
        {
            if (report.report[i] != 0)
            {
                bool newKey = true;
                for (int j = 1; j < 7; j++)
                {
                    if (prev[j] == report.report[i])
                    {
                        newKey = false;
                        break;
                    }
                }
                if (newKey)
                {
                    keyboard_HID2Ascii(report.report[i], report.report[0], true);
                }
            }
        }

        // Key Released
        for (int i = 1; i < 7; i++)
        {
            if (prev[i] != 0)
            {
                bool released = true;
                for (int j = 1; j < 7; j++)
                {
                    if (prev[i] == report.report[j])
                    {
                        released = false;
                        break;
                    }
                }
                if (released)
                {
                    keyboard_HID2Ascii(prev[i], prev[0], false);
                }
            }
        }

        memcpy(prev, report.report, 7);
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
        NimBLEDevice::setSecurityAuth(true, true, true); // bonding + MITM + SC

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

        NimBLEConnInfo connInfo = pClient->getConnInfo();

        //
        _log("  Peer Address: %s\n", pClient->getPeerAddress().toString().c_str());
        _log("  Conn Handle: %d\n", connInfo.getConnHandle());
        _log("  MTU: %u\n", pClient->getMTU());
        _log("  Encrypted: %s\n", connInfo.isEncrypted() ? "Yes" : "No");
        _log("  Bonded: %s\n", connInfo.isBonded() ? "Yes" : "No");

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

    /**
     * @brief Called when server requests to update the connection parameters.
     * @param [in] pClient A pointer to the calling client object.
     * @param [in] params A pointer to the struct containing the connection parameters requested.
     * @return True to accept the parameters.
     */
    bool onConnParamsUpdateRequest(NimBLEClient *pClient, const ble_gap_upd_params *params)
    {
        _log("[BLEClientCallbacks] onConnParamsUpdateRequest\n");
        _log("  Interval Min: %u (%.2f ms)\n", params->itvl_min, params->itvl_min * 1.25);
        _log("  Interval Max: %u (%.2f ms)\n", params->itvl_max, params->itvl_max * 1.25);
        _log("  Latency: %u events\n", params->latency);
        _log("  Supervision Timeout: %u (%.1f ms)\n",
             params->supervision_timeout,
             params->supervision_timeout * 10.0);

        return true; // Accept the parameters
    }

    /**
     * @brief Called when server requests a passkey for pairing.
     * @param [in] connInfo A reference to a NimBLEConnInfo instance containing the peer info.
     */
    void onPassKeyEntry(NimBLEConnInfo &connInfo)
    {
        Serial.println("Keyboard requesting passkey...");
        // Inject the same passkey you configured earlier
        NimBLEDevice::injectPassKey(connInfo, 123456);
    }

    /**
     * @brief Called when the pairing procedure is complete.
     * @param [in] connInfo A reference to a NimBLEConnInfo instance containing the peer info.\n
     * This can be used to check the status of the connection encryption/pairing.
     */
    void onAuthenticationComplete(NimBLEConnInfo &connInfo)
    {
        _log("[onAuthenticationComplete]\n");
        _log("  Peer Address: %s\n", connInfo.getAddress().toString().c_str());
        _log("  Encrypted: %s\n", connInfo.isEncrypted() ? "Yes" : "No");
        _log("  Bonded: %s\n", connInfo.isBonded() ? "Yes" : "No");
        _log("  Authenticated (MITM): %s\n", connInfo.isAuthenticated() ? "Yes" : "No");
        //_log("  Security Level: %d\n", connInfo.getSecurityLevel());
        //_log("  Key Size: %d\n", connInfo.getKeySize());
        //_log("  Role: %s\n", connInfo.getRole() == BLE_HS_CONN_ROLE_MASTER ? "Central" : "Peripheral");

        // save the address
        JsonDocument &app = status();
        app["config"]["ble"]["address"] = connInfo.getAddress().toString().c_str();
        config_save();
    }

    /**
     * @brief Called when using numeric comparision for pairing.
     * @param [in] connInfo A reference to a NimBLEConnInfo instance containing the peer info.
     * @param [in] pin The pin to compare with the server.
     */
    void onConfirmPasskey(NimBLEConnInfo &connInfo, uint32_t pin)
    {
        _log("[BLEClientCallbacks] onConfirmPasskey\n");
        Serial.printf("Confirm passkey: %06u\n", pin);
        // Accept automatically
        NimBLEDevice::injectConfirmPasskey(connInfo, true);
    }

    /**
     * @brief Called when the peer identity address is resolved.
     * @param [in] connInfo A reference to a NimBLEConnInfo instance with information
     */
    void onIdentity(NimBLEConnInfo &connInfo)
    {
        _log("[BLEClientCallbacks] onIdentity\n");
    }

    /**
     * @brief Called when the connection MTU changes.
     * @param [in] pClient A pointer to the client that the MTU change is associated with.
     * @param [in] MTU The new MTU value.
     * about the peer connection parameters.
     */
    void onMTUChange(NimBLEClient *pClient, uint16_t MTU)
    {
        _log("[BLEClientCallbacks] onMTUChange\n");
        _log("  Peer: %s\n", pClient->getPeerAddress().toString().c_str());
        _log("  New MTU: %u bytes\n", MTU);
    }

    /**
     * @brief Called when the PHY update procedure is complete.
     * @param [in] pClient A pointer to the client whose PHY was updated.
     * about the peer connection parameters.
     * @param [in] txPhy The transmit PHY.
     * @param [in] rxPhy The receive PHY.
     * Possible values:
     * * BLE_GAP_LE_PHY_1M
     * * BLE_GAP_LE_PHY_2M
     * * BLE_GAP_LE_PHY_CODED
     */
    void onPhyUpdate(NimBLEClient *pClient, uint8_t txPhy, uint8_t rxPhy)
    {
        _log("[BLEClientCallbacks] onPhyUpdate\n");
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
    // when key press message comes from the BLE keyboard
    // send it to the main process Q
    HidReport_t report;
    memcpy(report.report, pData, length > 7 ? 7 : length);
    xQueueSendFromISR(hidQueue, &report, nullptr);
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
