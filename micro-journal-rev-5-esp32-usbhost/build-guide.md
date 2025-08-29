

# DIY Build Guide for Micro Journal Rev.5 

This document serves as a build guide to assist anyone in constructing this device on their own. I'll provide the steps necessary to achieve this goal. If you find any key information missing, please don't hesitate to reach out to me.

## Component used

- [ESP32 S3 WROOM1 N16R8](https://www.amazon.it/gp/product/B0CLD4QKT1/ref=ppx_yo_dt_b_asin_title_o02_s00?ie=UTF8&psc=1)

Any ESP32-S3 devkit board can be utilized, and there's no necessity for it to be restricted to the specific postfix N16R8. These postfixes specify the flash space and PSRAM. The firmware is designed to operate with ESP32 Wroom boards, which have less flash and RAM, so any choice of S3 boards should suffice.

- [ILI9341 2.8" ILI9341 240 \* 320 TFT LCD Screen](https://www.amazon.it/ILI9341-Display-seriale-Arduino-Raspberry/dp/B07YTWRZGR/ref=sr_1_1?dib=eyJ2IjoiMSJ9.UQY1rH8qtdwW9a5QPrG3Tb0myeb_jE8vdzWnejFGWDF3RMdaKR2OYT2rD4zTsk5tRCPYmq4ARJ2DOFTW3cxicAmdQunZ0KTyhWrjZR2bVFiKfQjQvtB_BBvMuu2iOIYuM5oSUVJbFG4PloP041StmO_S7FB3SfJmuK9EX6EGx2Dkl0-VV2hkXMS2zlSrDwb7tUdrxbFU4Xl3W1rrLyoCOELgsObCdZHV0HMCGTORkiuCulcPIQpaLsI-kV6qgcPjFT8sMMJxv6foRX9yQPRCQHX4yDZGiHglS-64ot6zReg.JuwwXYaGqgeyWqYPXEaeERt-l1lEKuZ_2eHDJDNbUe4&dib_tag=se&keywords=ili9431&qid=1713525525&sr=8-1)

The display module includes an integrated SD card slot, eliminating the need for an additional SD card module. My 3D design utilizes the SD card slot located in the middle of the board. There are several variations of the ILI9341, so if you plan to use the provided STL files, ensure compatibility by checking dimensions with the linked product.

- [LiPo Charge Controller and 5V output module](https://www.amazon.it/Ouitble-caricabatteria-integrato-scarico-caricabatterie/dp/B09MQ6ND2Q/ref=sr_1_12?crid=8O78303YIA7M&dib=eyJ2IjoiMSJ9.ogstJhP7PWO-g81-wnxKh6VUbIZRJhLdyBc18r5jSRC5YsVWLrFZEKaf5mNfcg3UK9LZdo_N8zQ_iPBbUhlwr3lt_ZKxrB8_a-TVVGLO5m8y3xMHqBYbCl5NF8diqwd4Kg68cxOloVVTeY8NWgio7k0ON1HuFfkIrApVw4revVO-RqwcBmSR8IBHGI2L8TYDwmDItWB_bpQt9TZzSWhU2c5RgGiAslF4rDhHRQsu67HdDcQBRHaJdhroxiRJUNBHL3CnXyxrpiOCMKIH7pG8nBPgcX4mhEASN-Sw3NmjFsQ.qikh6iFDKhjEch8EOAYNdKsQmJk30ucZsf9OFwOfRzg&dib_tag=se&keywords=lipo+charger+module&qid=1711365173&sprefix=lipo+charger+module%2Caps%2C145&sr=8-12)

This is the simplest solution I found for resolving power supply issues for any 5V projects with batteries. It's straightforward and highly versatile for other voltage projects beyond 5V. This module is incredibly useful to have on hand.

- SD Card

I utilized a micro SD card in this project. I noticed that some SanDisk cards were not recognized. It's possible that some high-performance, ultra-fast cards may not have SPI interface compatibility... it's uncertain. It's advisable to purchase a card that is not overly fancy, preferably under 32GB. 1GB should provide more than enough storage.

If you purchase an SD card larger than 32GB, you'll need to create a partition with a maximum size of 32GB due to the ESP32's architectural limitation of being 32-bit, preventing it from reading larger than 32GB.

- USB-C female adaptor, Push button (6 mm), SPDT position switch
- [8x M3 heat inserts Length 3mm](https://www.aliexpress.com/item/1005004535859664.html)
- [8x M3 hex screw length 10mm](https://www.aliexpress.com/item/32442183034.html)
- [2x M2 Machine Screw length 6mm](https://www.aliexpress.com/item/1005004177352184.html)


- You will need TORX T10H to handle Hex screws 

- Any typical wires for electronics would do. I use [Wires 30 AWG](https://it.aliexpress.com/item/1005007081117235.html)


## Step 1. Preparing the DISPLAY

Remove the pins soldered on the display board using a nipper. Proceed slowly and gently to avoid scratching the board surface with the tools.

<img src="./images/011.jpg" width=400>

Apply additional solder to the pins, then use the nipper to remove them while melting the solder. Ensure to add extra solder when removing the pins.

<img src="./images/012.jpg" width=400>

Wire the MOSI, MISO, and CLK (SCK) pins from the SD card side of the pins to the DISPLAY part of the pins. Assign the CS pin of the SD card to the ESP32 board.

<img src="./images/013.jpg" width=400>

We will utilize the ILI9341 and SD card slot in this module, both utilizing the SPI interface. Therefore, the MOSI, MISO, and CLK pins are shared between the DISPLAY and SD functions.

Expose the following display pins as 10 cm wires from the board, which will later be connected to the ESP32 board:

- MISO
- MOSI
- SCLK
- DC
- RST
- VCC - LED combine these two together
- GND
- CS DISPLAY
- CS SD

<img src="./images/014.jpg" width=400>

Once done the board should like the picture above.



## Step 2. Power Supply

The power supply wiring revolves around the LiPo charger module.

Connect the OUT port to the position switch and a connector. This connector will supply power to the ESP32.

Connect the IN port to a USB-C female connector. This allows you to use USB-C cables, which are more readily available than micro USB cables these days.

Connect the BATT port to any battery case. In my build, I used an 18650 battery, but you can use any LiPo battery.

<img src="./images/019.png" width=800>

Above image from DessyTab who sufferred (ing) immensely at figuring out the wiring for the power supply. It's not the most accurate images. But should give enough clue to find the wiring done. Becareful with the Positive (+) wired to the positives, and Negatives (-) wired to the GND. 


## Step 3. Preparing USB-A female connector

USB-A female connector is needs to accept USB connection from the keyboard. You can search with "USB A female connector" to find the modules that is used in rev.5.

<img src="./images/usb-a-female.png" width=400>

https://www.aliexpress.com/item/4001146122421.html

They should come with 4 connections. 

| PIN  | PURPOSE                          |
| ---- | -------------------------------- |
| GND  | GND from the power supply module |
| VBUS | 5V from the sliding switch       |
| D-   | Wired to ESP32 pin 19            |
| D+   | Wired to ESP32 pin 20            |



## Step 3. Wiring to ESP32

Solder wires to the ESP32 board according to the pinout.

| PIN  | PURPOSE                    |
| ---- | -------------------------- |
| GND  | Power supply (5V rail) GND |
| 5Vin | Power supply               |
| 13   | MISO - SD, DISPLAY         |
| 12   | SCLK - SD, DISPLAY         |
| 11   | MOSI - SD, DISPLAY         |
| 10   | CS - DISPLAY               |
| 9    | CS - SD                    |
| 46   | DC - DISPLAY               |
| 3    | RST - DISPLAY              |
| 16   | BACK BUTTON (OPTIONAL)     |
| 15   | MENU BUTTON (OPTIONAL)     |
| 3V3  | DISPLAY VCC                |

| PIN | PURPOSE                   |
| --- | ------------------------- |
| GND | GND of BACK, MENU BUTTONS |
| GND | DISPLAY GND               |
| 19  | USB D-                    |
| 20  | USB D+                    |

<img src="./images/005.jpg" width=400>

Once the wiring is completed, it may resemble a tangled mess of wires, often referred to as "spaghetti."

## Step 4. Enclosure

<img src="./images/016.jpg" width=400>

STL files can be found in the folder "STL".

[STL FILES](https://github.com/unkyulee/micro-journal/tree/main/micro-journal-rev-5-esp32-usbhost/STL)

## Step 5. Compile and upload Firmware

The firmware for this iteration utilizes the same codebase as the previous version. To access the project repository for the micro-journal firmware, clone the repository from the following link:

https://github.com/unkyulee/micro-journal

Once cloned, navigate to the "/micro-journal-rev-4-esp32" folder to find the PlatformIO source project.

To work with the project, open it using Visual Studio with the PlatformIO plugin installed. From there, you'll be able to compile and upload the firmware to the ESP32-S3.

<img src="./images/017.jpg" width=400>

You may need to modify the /src/service/keyboard/usb/usb.cpp file and adjust the usbHost.setHIDLocal() function call to match your keyboard layout.

- Make sure to run Platform -> Upload Filesystem Image at least once, in order to set SPIFFS

## Step 6. Setup Google Apps Script for Drive Sync

To enable syncing to your Google Drive, you'll need to install a Google Apps Script on your Google Drive and obtain its URL. The code for the Google Apps Script can be found in the following path:

[/micro-journal-rev-4-esp32/install/google/sync.js](/micro-journal-rev-4-esp32/google/sync.js)

Create a new Google Apps Script document in your Google Drive, then copy the provided code into it. Deploy the script as a web app, which will generate a URL linked to your Google Drive. Place this URL in the "url" field of the config.json file. Once configured, the device should be able to sync files to your Google Drive seamlessly.

## Step 7. Steup SD card for WIFI access

Once all the components are assembled and the SD card is prepared, place a file named "config.json" inside the SD card with the following content:

```config.json
{
  "network": {
    "type": "wifi",
    "access_points": [
      {
        "ssid": "WIFI AP NAME",
        "password": "WIFIPASSWORD"
      }
    ]
  },
  "sync": {
    "url": "Google App Script URL goes here"
  }
}
```

Update the contents of the file with your WiFi connection information. This configuration will be saved on the SD card and utilized when sync is activated.
