# Bill of Material

* [FPC Adapter - 0.5-2.54MM, 24P](https://ko.aliexpress.com/item/1005007617729176.html)
* [Display Module](https://ko.aliexpress.com/item/1005012114591531.html)


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


| DISPLAY           | ESP32 |
| ----------------- | ----- |
| 16 (9) - VCI      | 3.3V  |
| 17 (8) - GND      | GND   |
| 13 (12) - SCLK    | 12    |
| 14 (11) - SDI     | 11    |
| 12 (13) - CS      | 10    |
| 11 (14) - LCD DC  | 46    |
| 10 (15) - LCD_RES | 3     |

                    