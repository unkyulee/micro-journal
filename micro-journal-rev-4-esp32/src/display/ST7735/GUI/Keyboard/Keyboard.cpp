#include "Keyboard.h"
#include "app/app.h"
#include "display/display.h"

//
void KeyboardScreen_setup(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    ptft->fillScreen(TFT_BLACK);
}

//
void KeyboardScreen_render(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    ///
    ptft->setTextSize(2); // Adjust text size as needed
    ptft->setTextColor(TFT_WHITE, TFT_BLACK);

    // Draw text in the middle of the screen
    ptft->setCursor(0, 0);
    ptft->print("\n");
    ptft->print("\n");
    ptft->print("USB Keyboard ");
}

void KeyboardScreen_keyboard(int key, bool pressed, int index)
{
    JsonDocument &app = status();
    _debug("KeyboardScreen_keyboard - %d %d %d\n", key, pressed, index);

    //
    if (index == 69)
    {
        if (!pressed)
        {
            _debug("KeyboardScreen_keyboard - Received MENU Key\n");
            app["screen"] = MENUSCREEN;
        }

        return;
    }
}