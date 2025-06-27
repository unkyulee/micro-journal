#include "ErrorScreen.h"
#include "app/app.h"

//
void ErrorScreen_setup(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    ptft->fillScreen(TFT_RED);
}

//
void ErrorScreen_render(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    // Text to be displayed
    JsonDocument &app = status();
    String text = app["error"].as<String>();

    ///
    ptft->setTextSize(2); // Adjust text size as needed
    ptft->setTextColor(TFT_WHITE, TFT_RED);

    // Draw text in the middle of the screen
    ptft->setCursor(0, 0);
    ptft->print(text);
}

//
void ErrorScreen_keyboard(char key)
{
}