#include "usb.h"
#include "app/app.h"
#include "app/config/config.h"
#include "service/display/display.h"
#include "service/keyboard/ascii/ascii.h"

//
#include "screens/WordProcessor/WordProcessor.h"
#include "screens/ErrorScreen/ErrorScreen.h"
#include "screens/Menu/Menu.h"
#include "screens/WakeUp/WakeUp.h"

void keyboard_bluetooth_setup() {}
void keyboard_bluetooth_loop() {}

/*
//
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include "BLE2902.h"
#include "BLEHIDDevice.h"
#include "HIDTypes.h"
#include "HIDKeyboardTypes.h"

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

const uint8_t keyboardHidDescriptor[] = {
    0x05, 0x0c,       // USAGE_PAGE (Consumer Devices)
    0x09, 0x01,       // USAGE (Consumer Control)
    0xa1, 0x01,       // COLLECTION (Application)
    0x85, 0x01,       //   REPORT_ID (1)
    0x19, 0x00,       //   USAGE_MINIMUM (Unassigned)
    0x2a, 0x3c, 0x02, //   USAGE_MAXIMUM (AC Format)
    0x15, 0x00,       //   LOGICAL_MINIMUM (0)
    0x26, 0x3c, 0x02, //   LOGICAL_MAXIMUM (572)
    0x95, 0x01,       //   REPORT_COUNT (1)
    0x75, 0x10,       //   REPORT_SIZE (16)
    0x81, 0x00,       //   INPUT (Data,Var,Abs)
    0xc0,             // END_COLLECTION
    0x05, 0x01,       // USAGE_PAGE (Generic Desktop)
    0x09, 0x06,       // USAGE (Keyboard)
    0xa1, 0x01,       // COLLECTION (Application)
    0x85, 0x02,       //   REPORT_ID (2)
    0x05, 0x07,       //   USAGE_PAGE (Keyboard)
    0x19, 0xe0,       //   USAGE_MINIMUM (Keyboard LeftControl)
    0x29, 0xe7,       //   USAGE_MAXIMUM (Keyboard Right GUI)
    0x15, 0x00,       //   LOGICAL_MINIMUM (0)
    0x25, 0x01,       //   LOGICAL_MAXIMUM (1)
    0x75, 0x01,       //   REPORT_SIZE (1)
    0x95, 0x08,       //   REPORT_COUNT (8)
    0x81, 0x02,       //   INPUT (Data,Var,Abs)
    0x95, 0x01,       //   REPORT_COUNT (1)
    0x75, 0x08,       //   REPORT_SIZE (8)
    0x25, 0x65,       //   LOGICAL_MAXIMUM (101)
    0x19, 0x00,       //   USAGE_MINIMUM (Reserved (no event indicated))
    0x29, 0x65,       //   USAGE_MAXIMUM (Keyboard Application)
    0x81, 0x00,       //   INPUT (Data,Ary,Abs)
    0xc0              // END_COLLECTION
};

typedef struct
{
    // uint8_t  reportId;                                 // Report ID = 0x02 (2)
    //  Collection: CA:ConsumerControl
    uint16_t ConsumerControl; // Value = 0 to 572
} inputConsumer_t;

static uint8_t idleRate; // in 4 ms units

typedef struct
{
    // uint8_t  reportId;                                 // Report ID = 0x02 (2)
    // Collection: CA:Keyboard
    uint8_t KB_KeyboardKeyboardLeftControl : 1;  // Usage 0x000700E0: Keyboard Left Control, Value = 0 to 1
    uint8_t KB_KeyboardKeyboardLeftShift : 1;    // Usage 0x000700E1: Keyboard Left Shift, Value = 0 to 1
    uint8_t KB_KeyboardKeyboardLeftAlt : 1;      // Usage 0x000700E2: Keyboard Left Alt, Value = 0 to 1
    uint8_t KB_KeyboardKeyboardLeftGui : 1;      // Usage 0x000700E3: Keyboard Left GUI, Value = 0 to 1
    uint8_t KB_KeyboardKeyboardRightControl : 1; // Usage 0x000700E4: Keyboard Right Control, Value = 0 to 1
    uint8_t KB_KeyboardKeyboardRightShift : 1;   // Usage 0x000700E5: Keyboard Right Shift, Value = 0 to 1
    uint8_t KB_KeyboardKeyboardRightAlt : 1;     // Usage 0x000700E6: Keyboard Right Alt, Value = 0 to 1
    uint8_t KB_KeyboardKeyboardRightGui : 1;     // Usage 0x000700E7: Keyboard Right GUI, Value = 0 to 1
    uint8_t Key;                                 // Value = 0 to 101
} inputKeyboard_t;

static inputConsumer_t consumer_Report{};
static inputKeyboard_t keyboard_report{}; // sent to PC

BLEHIDDevice *hid;
BLECharacteristic *input;
BLECharacteristic *output;
BLECharacteristic *inputVolume;
BLECharacteristic *outputVolume;
bool connected = false;

class MyCallbacks : public BLEServerCallbacks
{
    void onConnect(BLEServer *pServer)
    {
        connected = true;
        BLE2902 *desc = (BLE2902 *)input->getDescriptorByUUID(BLEUUID((uint16_t)0x2902));
        desc->setNotifications(true);

        BLE2902 *descv = (BLE2902 *)inputVolume->getDescriptorByUUID(BLEUUID((uint16_t)0x2902));
        descv->setNotifications(true);
    }

    void onDisconnect(BLEServer *pServer)
    {
        connected = false;
        BLE2902 *desc = (BLE2902 *)input->getDescriptorByUUID(BLEUUID((uint16_t)0x2902));
        desc->setNotifications(false);

        BLE2902 *descv = (BLE2902 *)inputVolume->getDescriptorByUUID(BLEUUID((uint16_t)0x2902));
        descv->setNotifications(false);
    }
};

void taskServer(void *)
{
    BLEDevice::init("ESP32");
    BLEServer *pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyCallbacks());

    hid = new BLEHIDDevice(pServer);
    inputVolume = hid->inputReport(1);   // <-- input REPORTID from report map
    outputVolume = hid->outputReport(1); // <-- output REPORTID from report map

    input = hid->inputReport(2);   // <-- input REPORTID from report map
    output = hid->outputReport(2); // <-- output REPORTID from report map

    std::string name = "ElectronicCats";
    hid->manufacturer()->setValue(name);

    hid->pnp(0x02, 0xe502, 0xa111, 0x0210);
    hid->hidInfo(0x00, 0x02);

    hid->reportMap((uint8_t *)keyboardHidDescriptor, sizeof(keyboardHidDescriptor));
    hid->startServices();

    BLESecurity *pSecurity = new BLESecurity();
    //  pSecurity->setKeySize();
    pSecurity->setAuthenticationMode(ESP_LE_AUTH_BOND);

    BLEAdvertising *pAdvertising = pServer->getAdvertising();
    pAdvertising->setAppearance(HID_KEYBOARD);
    pAdvertising->addServiceUUID(hid->hidService()->getUUID());
    pAdvertising->start();
    hid->setBatteryLevel(7);

    delay(portMAX_DELAY);
}

void keyboard_bluetooth_setup()
{
    //
    JsonDocument &app = app_status();
    bool enabled = app["config"]["bluetooth_enabled"].as<bool>();

    if (enabled)
    {
        app_log("Init Bluetooth Keyboard\n");

        //
        xTaskCreate(taskServer, "server", 20000, NULL, 5, NULL);
    }
}

void keyboard_bluetooth_loop()
{
    static unsigned int last = 0;
    if (millis() - last > 1000)
    {
        last = millis();

        static bool volDirUp = true;
        // put your main code here, to run repeatedly:

        if (connected)
        {
            inputKeyboard_t a{};
            a.Key = random(0x02, 0x27);
            //   a.reportId = 0x02;
            input->setValue((uint8_t *)&a, sizeof(a));
            input->notify();

            input->setValue((uint8_t *)(&keyboard_report), sizeof(keyboard_report));
            input->notify();

            inputConsumer_t b{};
            //   b.reportId = 0x01;

            b.ConsumerControl = volDirUp ? 0xE9 : 0xEA;
            volDirUp = volDirUp ? false : true;
            inputVolume->setValue((uint8_t *)&b, sizeof(b));
            inputVolume->notify();

            inputVolume->setValue((uint8_t *)&consumer_Report, sizeof(consumer_Report));
            inputVolume->notify();

            delay(100);
        }
    }
}
*/