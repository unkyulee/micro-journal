#include "Bluetooth.h"
#include "../Menu.h"
#include "app/app.h"
#include "config/config.h"
#include "display/display.h"
#include "display/WordProcessor/WordProcessor.h"
#include "keyboard/nimble/ble.h"

//
#include <SPI.h>
#include <SPIFFS.h>
#include <SD.h>

//
#define BLUETOOTH_CONFIG_LIST 0
#define BLUETOOTH_CONFIG_EDIT_ADDR 1
#define BLUETOOTH_CONFIG_POST_SCAN 2
int bluetooth_config_status = 0;
int bluetooth_config_index = 0;

#define BLUETOOTH_CONFIG_BUFFER 18
char bluetooth_config_buffer[BLUETOOTH_CONFIG_BUFFER];
int bluetooth_config_buffer_pos = 0;

static BLEScan *pBLEScan = nullptr;
static BLEScanResults results;
const int maxScanSize = 5;
std::string foundDevices[maxScanSize];
int deviceIndex = 0;

bool scanMode;

class advertisedDeviceCallback : public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice *advertisedDevice)
    {
        if (deviceIndex > maxScanSize)
        {
            app_log("BLE: Scan found too many keyboards");
            return;
        }
        for (int i = 0; i < advertisedDevice->getServiceUUIDCount(); i++)
        {
            BLEUUID service = advertisedDevice->getServiceUUID(i);
            if (service == serviceUUID)
            {
                std::string foundDevice = advertisedDevice->getAddress().toString();
                app_log("Keyboard found : %s\n", foundDevice.c_str());
                foundDevices[deviceIndex++] = foundDevice;
                return;
            }
        }
    }
};

void scan()
{
    // scan to run for 5 seconds.
    pBLEScan = BLEDevice::getScan();
    pBLEScan->setAdvertisedDeviceCallbacks(new advertisedDeviceCallback());
    pBLEScan->start(5);
    pBLEScan->clearResults();
    
}

void _bluetooth_saved_list(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f);
void _bluetooth_edit(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f);
void _bluetooth_select(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f);

//
void Bluetooth_setup(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    //
    Menu_clear();

    // 
    Bluetooth_load();

    //
    bluetooth_config_status = BLUETOOTH_CONFIG_LIST;

    // single bluetooth entry
    JsonDocument &app = app_status();
    JsonString savedAddress = app["config"]["ble"]["remote"].as<JsonString>();
    scanMode = app["config"]["ble"]["scan_mode"].as<bool>();

}

//
void Bluetooth_render(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    // Text to be displayed
    ptft->setCursor(0, 30, 2);
    ptft->setTextSize(1);

    //scan();
    //
    ptft->setTextColor(TFT_WHITE, TFT_BLACK);
    ptft->println(" BLUETOOTH SETTINGS ");
    ptft->println("");

    if (bluetooth_config_status == BLUETOOTH_CONFIG_LIST)
    {
        _bluetooth_saved_list(ptft, pu8f);
    }
    else if (bluetooth_config_status == BLUETOOTH_CONFIG_EDIT_ADDR)
    {
        _bluetooth_edit(ptft, pu8f);
    }
    else if (bluetooth_config_status == BLUETOOTH_CONFIG_POST_SCAN)
    {
        _bluetooth_select(ptft, pu8f);
    }
}

//
void Bluetooth_keyboard(char key)
{
    JsonDocument &app = app_status();

    if (bluetooth_config_status == BLUETOOTH_CONFIG_POST_SCAN)
    {
        if (key - '0' > 0 && key - '0' <= deviceIndex)
        {
            // picked a device
            app_log("Picked %d, %s\n", key, foundDevices[(key-1)-'0'].c_str());
            JsonObject ble = app["config"]["ble"].as<JsonObject>();
            ble["remote"] = foundDevices[(key-1)-'0'];
            Bluetooth_save();
        }
        else if (key == 'B' || key == 'b')
        {
            app_log("Returning without pick\n");
        }
        // reset
        for (int i = 0; i < deviceIndex; i++)
        {
            foundDevices[deviceIndex] = "";
        }
        deviceIndex = 0;
        bluetooth_config_status = BLUETOOTH_CONFIG_LIST;
    }
    else if (bluetooth_config_status == BLUETOOTH_CONFIG_EDIT_ADDR)
    {
        // BACK SPACE
        if (key == '\b')
        {
            // backspace
            bluetooth_config_buffer[bluetooth_config_buffer_pos--] = '\0';
        }
        else if (key == '\n')
        {
            // save address
            JsonObject ble = app["config"]["ble"].as<JsonObject>();
            ble["remote"] = String(bluetooth_config_buffer);

            bluetooth_config_buffer[0] = '\0';
            bluetooth_config_buffer_pos = 0;

            Bluetooth_save();

            //
            bluetooth_config_status = BLUETOOTH_CONFIG_LIST;
        }
        else
        {
            // edit mode
            bluetooth_config_buffer[bluetooth_config_buffer_pos++] = key;
            bluetooth_config_buffer[bluetooth_config_buffer_pos] = '\0';

        }

        // OVERFLOW CONTROL
        if (bluetooth_config_buffer_pos > BLUETOOTH_CONFIG_BUFFER)
        {
            bluetooth_config_buffer[0] = '\0';
            bluetooth_config_buffer_pos = 0;
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
            //
            // bluetooth entry chose to edit
            bluetooth_config_status = BLUETOOTH_CONFIG_EDIT_ADDR;
            bluetooth_config_buffer[0] = '\0';
            bluetooth_config_buffer_pos = 0;

            //
            Menu_clear();

            return;
        }
        else if (key == 'T' || key == 't')
        {
            //
            // Toggle scan mode
            app["config"]["ble"]["scan_mode"] = !app["config"]["ble"]["scan_mode"];
            Bluetooth_save();
            return;
        }
        else if ((key == 'S' || key == 's') && !ble_connected)
        {
            //
            // scan for bluetooth devices
            scan();
            if (deviceIndex > 0)
            {
                for (int i = 0; i < deviceIndex; i++)
                {
                    app_log("found device: %s\n", foundDevices[i].c_str());
                }
            }
            bluetooth_config_status = BLUETOOTH_CONFIG_POST_SCAN;

            return;
        }
        else if (key == 'X' || key == 'x')
        {
            // reboot
            ESP.restart();

        }
    }
}



void _bluetooth_saved_list(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    ptft->println(" EDIT REMOTE OR ENTER SCAN MODE: ");
    ptft->println("");

    //
    JsonDocument &app = app_status();

    // initialize the config
    if (!app["config"].containsKey("ble"))
    {
        JsonObject ble = app["config"]["ble"].to<JsonObject>();
        app["config"]["ble"] = ble;
    }

    if (!app["config"]["ble"].containsKey("remote"))
    {
        String ble_remote = app["config"]["ble"]["remote"].as<String>();
        app["config"]["ble"]["remote"] = ble_remote;

    }
    if (!app["config"]["ble"].containsKey("scan_mode"))
    {
        scanMode = app["config"]["ble"]["scan_mode"].as<bool>();
        app["config"]["ble"]["scan_mode"] = scanMode;
    }

    // get remote address
    String ble_remote = app["config"]["ble"]["remote"].as<String>();
    scanMode = app["config"]["ble"]["scan_mode"].as<bool>();

    // available ble remote
    ptft->printf(" [E] %s\n", ble_remote.c_str());

    ptft->printf(" [T] SCAN MODE: %d\n", scanMode);
    ptft->println(" SCAN MODE PREVENTS CONNECTION ON STARTUP ");

    if (!ble_connected)
    {
        ptft->println(" [S] SCAN (5 SECOND PAUSE)");
    }
    else
    {
        ptft->println(" SCAN DISABLED WHILE CONNECTED ");
    }

    ptft->println();
    ptft->println(" [X] RESTART ");
    ptft->println(" [B] BACK ");
}

void _bluetooth_edit(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    //
    JsonDocument &app = app_status();

    // Load saved BLE address
    String ble_remote = app["config"]["ble"]["remote"].as<String>();

    ptft->println(" EDIT BLE ADDRESS ");

    if (bluetooth_config_status == BLUETOOTH_CONFIG_EDIT_ADDR)
    {
        ptft->println(" TYPE ADDR:");
        ptft->println("");

        ptft->printf("      %s", bluetooth_config_buffer);
        ptft->println("");
        ptft->println("");
        ptft->println(" [ENTER] SAVE ");
    }
}

void _bluetooth_select(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    if (deviceIndex == 0)
    {
        ptft->println(" NO KEYBOARDS FOUND ");
        ptft->println("");
    }
    else
    {
        ptft->println(" SELECT BLE KEYBOARD ");
        for (int i = 0; i < deviceIndex; i++)
        {
            ptft->printf(" [%d] %s", i+1, foundDevices[i].c_str());
            ptft->println("");
        }
        ptft->println("");
    }

    ptft->println("[B] BACK");
}

void Bluetooth_load()
{
    //
    JsonDocument &app = app_status();

    // load config.json
    app_log("Opening config.json file\n");
    File file = SD.open("/config.json", "r");
    if (file)
    {
        // read the file
        app_log("Reading config.json file\n");
        String configString = file.readString();
        app_log("Closing config.json file\n");
        file.close();

        // check if configString is empty
        if (configString.isEmpty())
        {
            // to avoid deserialization failure whem empty
            configString = "{}";
        }

        // Prepare a JsonDocument for the configuration
        // The size should be adjusted according to your configuration's needs
        JsonDocument configDoc;

        // convert to JsonObject
        DeserializationError error = deserializeJson(configDoc, configString);
        app_log("Deserializing config.json file\n");
        if (error)
        {
            //
            app_log("config.json deserializeJson() failed: %s\n", error.c_str());

            //
            app["error"] = "Wrong format config.json";
            app["screen"] = ERRORSCREEN;

            return;
        }

// debug
#ifdef DEBUG
    app_log("%s\n", configString.c_str());
#endif

        // Assign the loaded configuration to "config" property of app
        app_log("Loading app status config\n");
        app["config"] = configDoc.as<JsonObject>();

        // print out the configuration
        app_log("BLE config loaded successfully!\n");
    }
    else
    {
        // file doesn't exist
        app_log("config.json file doens't exist\n");
        delay(100);

        return;
    }
}

void Bluetooth_save()
{
    // load app status
    JsonDocument &app = app_status();

    // save config
    // Open the file for writing
    File file = SD.open("/config.json", FILE_WRITE);
    if (!file)
    {
        app_log("Failed to open config.json file for writing.\n");
        return;
    }

    // Serialize the "config" property of the app Document directly to the file
    String jsonOutput;
    serializeJsonPretty(app["config"], jsonOutput);
    file.println(jsonOutput);

// debug
#ifdef DEBUG
    app_log("%s\n", jsonOutput.c_str());
#endif

    //
    app_log("BLE config updated successfully.\n");

    // close config.json
    file.close();
}