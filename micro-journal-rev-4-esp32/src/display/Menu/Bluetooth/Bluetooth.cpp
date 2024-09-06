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
int bluetooth_config_status = 0;
int bluetooth_config_index = 0;

#define BLUETOOTH_CONFIG_BUFFER 18
char bluetooth_config_buffer[BLUETOOTH_CONFIG_BUFFER];
int bluetooth_config_buffer_pos = 0;

static BLEScan *pBLEScan = nullptr;
static BLEScanResults results;
std::string foundDevices[5];

bool scanMode;

class advertisedDeviceCallback : public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice *advertisedDevice)
    {
        app_log("in result\n");
        app_log("%s (%s), type %d\n", advertisedDevice->getName().c_str(), advertisedDevice->getAddress().toString().c_str(), advertisedDevice->getAddress().getType());
        int deviceIndex = 0;
        for (int i = 0; i < advertisedDevice->getServiceUUIDCount(); i++)
        {
            BLEUUID service = advertisedDevice->getServiceUUID(i);
            app_log("service uuid: %s\n", service.toString().c_str());
            if (service == serviceUUID)
            {
                app_log("Keyboard found\n");
                std::string foundDevice = advertisedDevice->getAddress().toString();
                foundDevices[deviceIndex++] = foundDevice;
            }
        }
    }
};

void scan()
{
    app_log("SCANNING\n");
    // scan to run for 5 seconds.
    pBLEScan = BLEDevice::getScan();
    app_log("Got scan %x\n", pBLEScan);
    pBLEScan->setAdvertisedDeviceCallbacks(new advertisedDeviceCallback());
    pBLEScan->start(5);
    app_log("scan finished\n");
    app_log("got results\n");
    pBLEScan->clearResults();
    
}

void _bluetooth_saved_list(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f);
void _bluetooth_edit(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f);

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
    app_log("savedAddress from config: %s\r\n", savedAddress.c_str());
    scanMode = app["config"]["ble"]["scan_mode"].as<bool>();
    app_log("scanMode from config: %d\r\n", scanMode);

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
    else if (bluetooth_config_status >= BLUETOOTH_CONFIG_EDIT_ADDR)
    {
        _bluetooth_edit(ptft, pu8f);
    }
}

//
void Bluetooth_keyboard(char key)
{
    JsonDocument &app = app_status();

    if (bluetooth_config_status >= BLUETOOTH_CONFIG_EDIT_ADDR)
    {
        // BACK SPACE
        if (key == '\b')
        {
            // backspace
            bluetooth_config_buffer[bluetooth_config_buffer_pos--] = '\0';
        }
        else if (key == '\n')
        {
            // next
            if (bluetooth_config_status == BLUETOOTH_CONFIG_EDIT_ADDR)
            {
                // save address
                JsonObject ble = app["config"]["ble"].as<JsonObject>();
                ble["remote"] = String(bluetooth_config_buffer);
                app_log("ble[remote]: %s\n", ble["remote"]);

                bluetooth_config_buffer[0] = '\0';
                bluetooth_config_buffer_pos = 0;

                Bluetooth_save();

                //
                bluetooth_config_status = BLUETOOTH_CONFIG_LIST;
            }
        }
        else
        {
            // edit mode
            bluetooth_config_buffer[bluetooth_config_buffer_pos++] = key;
            bluetooth_config_buffer[bluetooth_config_buffer_pos] = '\0';

                app_log("ble_buffer: %s\n", bluetooth_config_buffer);

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
        else if ((key == 'S' || key == 's') && !ble_connected && scanMode)
        {
            //
            // scan for bluetooth devices
            scan();

            for (int i = 0; i < foundDevices->length(); i++)
            {
                app_log("found device: %s\n", foundDevices[i]);
            }

            return;
        }
    }
}



void _bluetooth_saved_list(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    ptft->println(" EDIT REMOTE OR ENTER SCAN MODE: ");
    ptft->println("");

    //
    JsonDocument &app = app_status();
    // String jsonOutput;
    // serializeJsonPretty(app["config"], jsonOutput);
    // app_log("_bt_saved_list: %s\n", jsonOutput.c_str());

    // initialize the config
    if (!app["config"].containsKey("ble"))
    {
        app_log("No BLE found, init");
        JsonObject ble = app["config"]["ble"].to<JsonObject>();
        app["config"]["ble"] = ble;
    }

    if (!app["config"]["ble"].containsKey("remote"))
    {
        app_log("No ble.remote found, init");
        String ble_remote = app["config"]["ble"]["remote"].as<String>();
        app["config"]["ble"]["remote"] = ble_remote;

    }
    if (!app["config"]["ble"].containsKey("scan_mode"))
    {
        app_log("No ble.scan_mode found, init");
        scanMode = app["config"]["ble"]["scan_mode"].as<bool>();
        app["config"]["ble"]["scan_mode"] = scanMode;
    }

    // get remote address
    // app_log("scan_mode_pre: %d\n", scanMode);
    String ble_remote = app["config"]["ble"]["remote"].as<String>();
    scanMode = app["config"]["ble"]["scan_mode"].as<bool>();

    // app_log("ble_remote: %s\n", ble_remote);
    // app_log("scan_mode_post: %d\n", scanMode);

    // available ble remote
    ptft->printf(" [E] %s\n", ble_remote);

    ptft->printf(" [T] SCAN MODE: %d\n", scanMode);
    ptft->println(" SET TRUE AND RESTART TO ALLOW SCANNING IF DEVICE IS CONNECTED ");

    if (!ble_connected && scanMode)
    {
        ptft->println(" [S] SCAN");
    }
    else
    {
        ptft->println(" CANNOT SCAN WHILE CONNECTED OR NOT IN SCAN MODE ");
    }

    ptft->println();
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