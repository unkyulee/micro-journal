#include "WakeUp.h"
#include "app/app.h"
#include "display/display.h"
#include "service/GIF/GIF.h"

bool _wakeup_gif_loaded = false;

void WakeUp_setup(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    //
    ptft->fillScreen(TFT_BLACK);

    //
    _wakeup_gif_loaded = gif_setup(ptft, pu8f, "/wakeup.gif", WORDPROCESSOR, true);
}
void WakeUp_render(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    JsonDocument &app = status();

    // Play GIF
    if (_wakeup_gif_loaded)
        gif_render(ptft, pu8f);
    else
        // move to next screen
        app["screen"] = WORDPROCESSOR;
}

void WakeUp_keyboard(int key, bool pressed, int index)
{
    // any key press will stop the gif
    gif_stop();
}