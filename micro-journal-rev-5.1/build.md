DRAFT

# Micro Journal Rev.5.1. Build Guide

Internals of Rev.5.1. is the same as the previous Rev.5 build. Please, refer to the [Micro Journal Rev.5 Build Guide](../micro-journal-rev-5-esp32-usbhost/build-guide.md) for the detailed steps. Components used and 3D printed enclosure in the Rev.5.1. are slightly different.


## Bill of Material

* [ESP32-S3 N16R8](https://www.amazon.com/Development-AYWHP-ESP32-S3-DevKitC-WROOM-1-N16R8-Compatible/dp/B0DG8L7MQ9)
  – Any ESP32-S3 devkit will work (not limited to N16R8). The firmware also runs on ESP32 Wroom boards with less flash/PSRAM.

* [ILI9341 2.8" TFT LCD (240x320)](https://ko.aliexpress.com/item/1005006323532762.html)
  – Includes a built-in SD card slot (used in this build). Variations exist, so double-check dimensions if using the provided STL files.
  - If you can find the same dimension with ST7789 chip, it will still work and actually has a better quality. Just that it is a bit difficult to find than ILI9341

* [18650 LiPo Charge Controller](https://ko.aliexpress.com/item/1005006366996657.html)
* [18650 Battery Holder](https://www.aliexpress.com/item/1005005084346241.html)

* **Micro SD card** Size less than 32GB recommended
  – Avoid ultra-fast cards; simple cards work better. If using >32GB, create a partition ≤32GB (ESP32 is 32-bit and can't read larger).

* [SPST Snap in Switch ON Off 2Pin 19mm](https://ko.aliexpress.com/item/1005008528747478.html)

* [USB Socket Terminal 4 Wires](https://ko.aliexpress.com/item/1005005576978983.html)

* M3 Heated Inserts OD 4.5mm Length 3mm
* M2 Heated Inserts OD 3.2mm Length 3mm

* DIN 912 M3 Hex Screw Length 10mm
* DIN 912 M3 Hex Screw Length 5mm
* DIN 7046 M2 Machine Screw Length 5mm

* [8x M3 hex screws (10mm length)](https://www.aliexpress.com/item/32442183034.html)

* [2x M2 machine screws (6mm length)](https://www.aliexpress.com/item/1005004177352184.html)

- You will need TORX T10H to handle Hex screws
- Any typical wires for electronics would do. I use [Wires 30 AWG](https://it.aliexpress.com/item/1005007081117235.html)



## USB Cable Reference

Next to the Red wire (Vcc) is D-
Next to the Black wire (GND) is D+

| Pin  | Purpose                |
| ---- | ---------------------- |
| GND  | To power supply GND    |
| VBUS | 5V from sliding switch |
| D-   | ESP32 pin 19           |
| D+   | ESP32 pin 20           |


## Some more Reference Images

<img src="./images/rev 5 1 floorplan_display blank.png" />

<img src="./images/rev 5 1 floorplan_2.png" />