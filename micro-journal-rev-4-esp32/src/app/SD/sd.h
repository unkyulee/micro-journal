#ifndef SD_h
#define SD_h

#ifdef ENV_ILI9341
#define SD_CS 9
#endif
#ifdef ENV_EPAPER
#define SD_MISO    (16)
#define SD_MOSI    (15)
#define SD_SCLK    (11)
#define SD_CS      (42)
#endif

//
void SD_setup();

#endif