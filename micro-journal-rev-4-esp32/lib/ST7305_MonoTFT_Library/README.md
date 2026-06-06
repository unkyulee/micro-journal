# ST7305_MonoTFT_Library

 

### Default pins
These macros live in `DisplayConfig.h` (override as needed):
```c
#define PIN_DC    4
#define PIN_CS    3
#define PIN_SCLK  2
#define PIN_SDIN  1
#define PIN_RST   0
```
Use the convenience constructors that read from these defaults:
```cpp
ST7305_2p9_BW_DisplayDriver lcd(spi);   // 2.9"
ST7305_4p2_BW_DisplayDriver lcd(spi);   // 4.2"
```
