#include "service.h"
#include "display/display.h"

void service_setup_main()
{
    display_setup_main();
}

void service_setup_secondary() 
{
    display_setup_secondary();
}

// place the service loops that is critical and should be performed in timely manner
void service_loop_main()
{
    display_loop_main();
}

// place the service loops that doesn't affect the main functionality
void service_loop_secondary()
{
    display_loop_secondary();
}
