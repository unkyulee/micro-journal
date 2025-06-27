#include "ErrorScreen.h"
#include "app/app.h"

//
void ErrorScreen_setup(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    ptft->fillScreen(TFT_BLACK);
}

//
void ErrorScreen_render(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    // Text to be displayed
    JsonDocument &app = status();
    String text = app["error"].as<String>();

    ///
    ptft->setCursor(0, 0, 2);
    ptft->setTextSize(1); // Adjust text size as needed
    ptft->setTextColor(TFT_WHITE, TFT_RED);

    // Draw text in the middle of the screen
    ptft->setCursor(0, 0, 2);
    ptft->print(text);
}

//
void ErrorScreen_keyboard(char key)
{
    // Any key press reset ESP32
    ESP.restart(); // Reset the ESP32
    delay(1000);
}