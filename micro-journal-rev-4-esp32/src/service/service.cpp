#include "service.h"
#include "display/display.h"
#include "keyboard/keyboard.h"
#include "SD/sd.h"
#include "WordProcessor/WordProcessor.h"

void service_setup()
{
    // SD must be initialized before display
    SD_setup();
    display_setup();
    keyboard_setup();

    // Word Processor Setup
    WP_setup();
}

// place the service loops that is critical and should be performed in timely manner
void service_loop()
{
    display_loop();
    keyboard_loop();
    SD_loop();

    // Word Processor Loop
    WP_loop();
}
