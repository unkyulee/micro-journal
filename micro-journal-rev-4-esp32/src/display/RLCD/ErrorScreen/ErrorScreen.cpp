#include "ErrorScreen.h"
#include "app/app.h"

//
void ErrorScreen_setup(ST7305_4p2_BW_DisplayDriver *display, U8G2_FOR_ST73XX *u8)
{
    //    
    _log("Error Screen Setup\n");

    //
    display->clearDisplay();
}

//
void ErrorScreen_render(ST7305_4p2_BW_DisplayDriver *display, U8G2_FOR_ST73XX *u8)
{
    // Text to be displayed
    JsonDocument &app = status();
    String text = app["error"].as<String>();

    ///
    u8->setFont(u8g2_font_profont22_tf);
    u8->setCursor(0, 10);
    u8->print(text);
}

//
void ErrorScreen_keyboard(char key)
{
    // Any key press reset ESP32
    _log("Restarting ESP32\n");
    ESP.restart(); // Reset the ESP32
}