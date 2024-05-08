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
    JsonDocument &app = app_status();
    String text = app["error"].as<String>();

    ///
    ptft->setCursor(0, 0, 2);
    ptft->setTextSize(2); // Adjust text size as needed
    ptft->setTextColor(TFT_WHITE, TFT_RED);

    // Calculate text dimensions
    uint16_t textWidth = ptft->textWidth(text.c_str());
    uint16_t textHeight = ptft->fontHeight();

    // Calculate the position to draw text in the middle of the screen
    int16_t posX = (ptft->width() - textWidth) / 2;
    int16_t posY = (ptft->height() - textHeight) / 2;

    // Draw text in the middle of the screen
    ptft->setCursor(posX, posY, 2);
    ptft->print(text);
}

//
void ErrorScreen_keyboard(char key)
{
    // Any key press reset ESP32
    ESP.restart(); // Reset the ESP32
    delay(1000);
}