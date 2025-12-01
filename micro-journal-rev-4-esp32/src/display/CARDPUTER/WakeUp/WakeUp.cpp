#include "WakeUp.h"
#include "app/app.h"
#include "display/display.h"

//
void WakeUp_setup()
{
    _log("Wakup Screen Setup\n");

    // move to word processor
    JsonDocument &app = status();
    app["screen"] = WORDPROCESSOR;
}

//
void WakeUp_render()
{
}

//
void WakeUp_keyboard(char key)
{
}
