Detailed build guides will be added later, once the build has stabilized.

For now, I will provide the bill of materials and wiring guides, so people who are comfortable figuring out the remaining details on their own can still build it at this stage.

# Bill of Material

- [Reflective LCD Display Module](https://ko.aliexpress.com/item/1005012114591531.html)
- [FPC Adapter - 0.5-2.54MM, 24P](https://ko.aliexpress.com/item/1005007617729176.html)
- ESP32 S3 N16R8
- [LiPo Charger and Step Up Controller](https://www.aliexpress.com/item/1005006366996657.html)
- [18650 Battery Holder](https://www.aliexpress.com/item/1005005084346241.html)
- [2 Pin Round Snap Rocker Switch 19mm](https://it.aliexpress.com/item/1005008528747478.html)
- [69 Keyboard PCB](https://www.elecrow.com/micro-journal-diy-kit-68-keys-keyboard-pcb.html)
- Costar Stabilizer 6.25u


- M3 Heated Inserts OD 4.5mm Length 3mm
- M2 Heated Inserts OD 3.2mm Length 3mm
- DIN 912 M3 Hex Screw Length 5mm
- DIN 912 M3 Hex Screw Length 10mm
- DIN 912 M3 Hex Screw Length 70mm
- DIN 7046 M2 Machine Screw Length 5mm

- [B-7000 Glue](https://www.aliexpress.com/item/1005005379063116.html)
- You will need TORX T10H to handle Hex screws
- Any typical wires for electronics would do. I use [Wires 30 AWG](https://it.aliexpress.com/item/1005007081117235.html)


# Reference

## Display Pin Mapping

Display ST7306
300 x 400 dot matrix TFT LCD module
YDP420H001-V3
Osptek Display

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
| 16 - VCI     | Power dispply (analog) VCI 2.55 - 3.3V                                   |
| 17 - GND     | Power GND                                                                |


3.3V from ESP32 to pin 16
GND to pin 17

Breakout board has different PIN number on the front and the back.
I used the pin number marked inside the bracket in this build.

| DISPLAY           | ESP32 |
| ----------------- | ----- |
| 16 (9) - VCI      | 3.3V  |
| 17 (8) - GND      | GND   |
| 13 (12) - SCLK    | 12    |
| 14 (11) - SDI     | 11    |
| 12 (13) - CS      | 10    |
| 11 (14) - LCD DC  | 46    |
| 10 (15) - LCD_RES | 3     |



## Keyboard to ESP32

Uses only the middle pin pad. Start from the squared pin and 17 pins are used.
Starts from ROWS, and then COLS

These are pins maps to ESP32 in order.
byte rowPins[ROWS] = {8, 18, 17, 16, 15, 7, 6, 5};
byte colPins[COLS] = {1, 2, 42, 41, 40, 39, 45, 48, 47};

                    

## Support

If Micro Journal helped you build something you love, consider buying me a coffee through the link below. It is a small gesture, but it helps keep the project alive, cared for, and open for everyone.

* [Buy me a coffee](https://www.buymeacoffee.com/unkyulee)  


Un Kyu Lee