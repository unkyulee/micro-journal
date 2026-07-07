#include "MassStorage.h"
#include "app/app.h"

#ifdef BOARD_PICO
#include "rp2040/MassStorageRP2040.h"
#endif

#ifdef BOARD_ESP32_S3
#include "esp32/MassStorageESP32.h"
#endif

//
void ms_setup()
{
    // Register callback for host ejection
    _log("Mass Storage Setup\n");

#ifdef BOARD_PICO
    ms_rp2040_setup();
#endif
}

//
void ms_loop()
{
#ifdef BOARD_PICO
    ms_rp2040_loop();
#endif

#ifdef BOARD_ESP32_S3
    ms_esp32_loop();
#endif    
}