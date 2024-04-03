#include "keyboard.h"
#include "app/app.h"
#include "app/network/network.h"
#include "service/display/display.h"

//
#include "screens/WordProcessor/WordProcessor.h"
#include "screens/ErrorScreen/ErrorScreen.h"
#include "screens/Menu/Menu.h"

//
#include <ESP32-USB-Soft-Host.h>

#define DP_P0 12 // always enabled
#define DM_P0 14 // always enabled
#define DP_P1 22 // -1 to disable
#define DM_P1 23 // -1 to disable
#define DP_P2 18 // -1 to disable
#define DM_P2 19 // -1 to disable
#define DP_P3 13 // -1 to disable
#define DM_P3 15 // -1 to disable

static void my_USB_DetectCB(uint8_t usbNum, void *dev)
{
    sDevDesc *device = (sDevDesc *)dev;
    printf("New device detected on USB#%d\n", usbNum);
    printf("desc.bcdUSB             = 0x%04x\n", device->bcdUSB);
    printf("desc.bDeviceClass       = 0x%02x\n", device->bDeviceClass);
    printf("desc.bDeviceSubClass    = 0x%02x\n", device->bDeviceSubClass);
    printf("desc.bDeviceProtocol    = 0x%02x\n", device->bDeviceProtocol);
    printf("desc.bMaxPacketSize0    = 0x%02x\n", device->bMaxPacketSize0);
    printf("desc.idVendor           = 0x%04x\n", device->idVendor);
    printf("desc.idProduct          = 0x%04x\n", device->idProduct);
    printf("desc.bcdDevice          = 0x%04x\n", device->bcdDevice);
    printf("desc.iManufacturer      = 0x%02x\n", device->iManufacturer);
    printf("desc.iProduct           = 0x%02x\n", device->iProduct);
    printf("desc.iSerialNumber      = 0x%02x\n", device->iSerialNumber);
    printf("desc.bNumConfigurations = 0x%02x\n", device->bNumConfigurations);
    // if( device->iProduct == mySupportedIdProduct && device->iManufacturer == mySupportedManufacturer ) {
    //   myListenUSBPort = usbNum;
    // }
}

static void my_USB_PrintCB(uint8_t usbNum, uint8_t byte_depth, uint8_t *data, uint8_t data_len)
{
    // if( myListenUSBPort != usbNum ) return;
    printf("in: ");
    for (int k = 0; k < data_len; k++)
    {
        printf("0x%02x ", data[k]);
    }
    printf("\n");
}

usb_pins_config_t USB_Pins_Config = {
    DP_P0, DM_P0};

// initialize USB HOST
void keyboard_setup()
{
    printf("TIMER_BASE_CLK: %d, TIMER_DIVIDER:%d, TIMER_SCALE: %d\n", TIMER_BASE_CLK, TIMER_DIVIDER, TIMER_SCALE);
    // USH.setTaskCore( 0 );
    // USH.setBlinkPin( (gpio_num_t) 2 );
    // USH.setTaskPriority( 16 );
    USH.setOnConfigDescCB(Default_USB_ConfigDescCB);
    USH.setOnIfaceDescCb(Default_USB_IfaceDescCb);
    USH.setOnHIDDevDescCb(Default_USB_HIDDevDescCb);
    USH.setOnEPDescCb(Default_USB_EPDescCb);

    USH.init(USB_Pins_Config, my_USB_DetectCB, my_USB_PrintCB);
}

///
void keyboard_loop()
{
    static unsigned int last = 0;
    if (millis() - last > 10)
    {
        //
        last = millis();
    }
}
