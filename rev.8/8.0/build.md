# Build Guide

This is the build guide for Micro Journal Rev.8.

> **Status:** This guide is a work in progress. Detailed step-by-step instructions will be added once the build has stabilized. For now, it provides the bill of materials and the wiring reference, so people who are comfortable figuring out the remaining details on their own can still build it at this stage. Sections that are not written yet are marked **TBD**.

- [Build Guide](#build-guide)
- [Requirements](#requirements)
- [Video Guide](#video-guide)
- [Bill of Materials](#bill-of-materials)
  - [Electronics](#electronics)
  - [Keyboard](#keyboard)
  - [Hardware](#hardware)
  - [3D Printed Parts](#3d-printed-parts)
- [Required Tools](#required-tools)
- [Build Order](#build-order)
- [1. Enclosure Preparation](#1-enclosure-preparation)
  - [Printing](#printing)
  - [Heated Inserts](#heated-inserts)
  - [Assembling the Printed Parts](#assembling-the-printed-parts)
- [2. Keyboard PCB](#2-keyboard-pcb)
  - [Soldering](#soldering)
  - [Stabilizer](#stabilizer)
- [3. Wire the Keyboard to the ESP32](#3-wire-the-keyboard-to-the-esp32)
- [4. Display](#4-display)
  - [Display Module](#display-module)
  - [Display Pins](#display-pins)
  - [Wiring the Display to the ESP32](#wiring-the-display-to-the-esp32)
  - [Mounting the Display](#mounting-the-display)
- [5. Power Supply](#5-power-supply)
- [6. Firmware](#6-firmware)
- [7. Closing the Enclosure](#7-closing-the-enclosure)
- [8. Switches and Keycaps](#8-switches-and-keycaps)
- [Troubleshooting](#troubleshooting)
- [Support](#support)

---

# Requirements

TBD

---

# Video Guide

TBD

---

# Bill of Materials

## Electronics

| Part | Notes |
| ---- | ----- |
| [Reflective LCD Display Module](https://ko.aliexpress.com/item/1005012114591531.html) | ST7306, 300 x 400, model YDP420H001-V3 |
| [FPC Adapter - 0.5-2.54MM, 24P](https://ko.aliexpress.com/item/1005007617729176.html) | Breakout board for the display cable |
| ESP32 S3 N16R8 | Microcontroller |
| [LiPo Charger and Step Up Controller](https://www.aliexpress.com/item/1005006366996657.html) | |
| [18650 Battery Holder](https://www.aliexpress.com/item/1005005084346241.html) | |
| [2 Pin Round Snap Rocker Switch 19mm](https://it.aliexpress.com/item/1005008528747478.html) | Power switch |
| [Wires 30 AWG](https://it.aliexpress.com/item/1005007081117235.html) | Any typical wires for electronics would do |

## Keyboard

| Part | Notes |
| ---- | ----- |
| [69 Keyboard PCB](https://www.elecrow.com/micro-journal-diy-kit-68-keys-keyboard-pcb.html) | |
| Costar Stabilizer 6.25u | For the spacebar |
| Keyboard switches | TBD |
| Keycaps | TBD |

## Hardware

| Part | Quantity |
| ---- | -------- |
| M3 Heated Inserts OD 4.5mm Length 3mm | TBD |
| M2 Heated Inserts OD 3.2mm Length 3mm | TBD |
| DIN 912 M3 Hex Screw Length 5mm | TBD |
| DIN 912 M3 Hex Screw Length 10mm | TBD |
| DIN 912 M3 Hex Screw Length 70mm | TBD |
| DIN 7046 M2 Machine Screw Length 5mm | TBD |
| [B-7000 Glue](https://www.aliexpress.com/item/1005005379063116.html) | |

## 3D Printed Parts

The [STL files](./STL) for the 3D prints are available in this repository. A combined project file, `rev.8.3mf`, is in the same folder.



---

# Required Tools

- Soldering iron
- TORX T10H to handle the hex screws
- Other tools: TBD

---

# Build Order

1. Prepare the enclosure
2. Build the keyboard PCB
3. Wire the keyboard to the ESP32
4. Wire the display
5. Wire the power supply
6. Install the firmware
7. Close the enclosure
8. Install switches and keycaps

---

# 1. Enclosure Preparation

## Printing

Print settings, material and support recommendations: TBD

## Heated Inserts

Insert locations: TBD

## Assembling the Printed Parts

TBD

---

# 2. Keyboard PCB

The keyboard PCB can be ordered from [Elecrow](https://www.elecrow.com/micro-journal-diy-kit-68-keys-keyboard-pcb.html).

Gerber files: TBD

## Soldering

TBD

## Stabilizer

The spacebar uses a Costar Stabilizer 6.25u.

Installation steps: TBD

---

# 3. Wire the Keyboard to the ESP32

The keyboard PCB connects to the ESP32 through the **middle pin pad** only. 17 pins are used, counting from the **squared pin**. The rows come first, followed by the columns.

Connect the pads to the ESP32 pins in this order:

| Pad (from the squared pin) | Signal | ESP32 pin |
| -------------------------- | ------ | --------- |
| 1  | ROW 1 | 8  |
| 2  | ROW 2 | 18 |
| 3  | ROW 3 | 17 |
| 4  | ROW 4 | 16 |
| 5  | ROW 5 | 15 |
| 6  | ROW 6 | 7  |
| 7  | ROW 7 | 6  |
| 8  | ROW 8 | 5  |
| 9  | COL 1 | 1  |
| 10 | COL 2 | 2  |
| 11 | COL 3 | 42 |
| 12 | COL 4 | 41 |
| 13 | COL 5 | 40 |
| 14 | COL 6 | 39 |
| 15 | COL 7 | 45 |
| 16 | COL 8 | 48 |
| 17 | COL 9 | 47 |

The same mapping as it appears in the firmware:

```cpp
byte rowPins[ROWS] = {8, 18, 17, 16, 15, 7, 6, 5};
byte colPins[COLS] = {1, 2, 42, 41, 40, 39, 45, 48, 47};
```

Wiring photos: TBD

---

# 4. Display

## Display Module

| Item | Value |
| ---- | ----- |
| Driver | ST7306 |
| Type | 300 x 400 dot matrix TFT LCD module |
| Model | YDP420H001-V3 |
| Manufacturer | Osptek Display |
| Driver reference code | https://gitee.com/osptek/4.2-lcd-300x400-spi-st7305 |

## Display Pins

| PIN          | Description                                                              |
| ------------ | ------------------------------------------------------------------------ |
| 5 - VCC      | Power Source 3.3V                                                        |
| 9 - TE       | Tearing effect signal is used to synchronize MCU to frame memory writing |
| 10 - LCD_RES | Reset                                                                    |
| 11 - LCD DC  | Data/Command selection                                                   |
| 12 - CS      | Chip Selection                                                           |
| 13 - SCLKS   | Clock                                                                    |
| 14 - SDI     | MOSI, SPI interface input/out pin                                        |
| 15 - IOVCC   | Power supply digital. IOVCC 1.65 - 3.3V                                  |
| 16 - VCI     | Power display (analog) VCI 2.55 - 3.3V                                   |
| 17 - GND     | Power GND                                                                |

## Wiring the Display to the ESP32

The display connects to the ESP32 through the FPC adapter (breakout board).

> **Note:** The breakout board has different pin numbers printed on the front and on the back. This build uses the pin number shown inside the brackets.

| DISPLAY           | ESP32 |
| ----------------- | ----- |
| 16 (9) - VCI      | 3.3V  |
| 17 (8) - GND      | GND   |
| 13 (12) - SCLK    | 12    |
| 14 (11) - SDI     | 11    |
| 12 (13) - CS      | 10    |
| 11 (14) - LCD DC  | 46    |
| 10 (15) - LCD_RES | 3     |

Power comes from the ESP32: 3.3V goes to display pin 16 and GND goes to display pin 17.

## Mounting the Display

TBD

---

# 5. Power Supply

The power supply uses the LiPo charger and step up controller, the 18650 battery holder, and the rocker switch.

Wiring diagram: TBD

Assembly steps: TBD

---

# 6. Firmware

The firmware source code is in the [micro-journal-mcu](https://github.com/unkyulee/micro-journal-mcu) repository. Firmware files are published on the [releases page](https://github.com/unkyulee/micro-journal/releases).

To install the firmware on a new board, follow the full web flash steps in [Update the Firmware](./guide.md#5-update-the-firmware).

Building the firmware from source: TBD

---

# 7. Closing the Enclosure

TBD

---

# 8. Switches and Keycaps

TBD

---

# Troubleshooting

TBD

---

# Support

If Micro Journal helped you build something you love, consider buying me a coffee through the link below. It is a small gesture, but it helps keep the project alive, cared for, and open for everyone.

* [Buy me a coffee](https://www.buymeacoffee.com/unkyulee)

Un Kyu Lee
