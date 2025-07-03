#include "Sleep.h"
#include "app/app.h"
#include "display/display.h"
#include "service/GIF/GIF.h"

bool _sleep_gif_loaded = false;

void Sleep_setup(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    //
    ptft->fillScreen(TFT_BLACK);

    //
    _sleep_gif_loaded = gif_setup(ptft, pu8f, "/Sleep.gif", WORDPROCESSOR, false);

    //
    _log("Entering Sleep Screen\n");

}
void Sleep_render(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    JsonDocument &app = status();

    // Play GIF
    if (_sleep_gif_loaded)
        gif_render(ptft, pu8f);
    else
        // move to next screen
        app["screen"] = WORDPROCESSOR;
}

void Sleep_keyboard(int key, bool pressed, int index)
{
    // any key press will stop the gif
    gif_stop();
}