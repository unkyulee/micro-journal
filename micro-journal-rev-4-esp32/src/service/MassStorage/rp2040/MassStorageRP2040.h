#pragma once

#ifdef BOARD_PICO

#ifndef USE_TINYUSB
#error MassStorageRP2040 requires -D USE_TINYUSB (all rp2040 builds use Adafruit TinyUSB)
#endif

void ms_rp2040_setup();
void ms_rp2040_loop();

#endif
