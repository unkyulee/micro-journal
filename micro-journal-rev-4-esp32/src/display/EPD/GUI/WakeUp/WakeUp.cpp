#include "WakeUp.h"
#include "../../display_EPD.h"

//
#include "app/app.h"
#include "display/display.h"
#include "keyboard/keyboard.h"

bool wakeup = true;
bool stop = false;

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
