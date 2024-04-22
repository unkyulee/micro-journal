#include "service.h"
#include "display/display.h"
#include "button/button.h"

void service_setup()
{
    //
    display_setup();

    // 
    button_setup();
}

// place the service loops that is critical and should be performed in timely manner
void service_loop()
{
    //
    display_loop();

    // 
    button_loop();
}
