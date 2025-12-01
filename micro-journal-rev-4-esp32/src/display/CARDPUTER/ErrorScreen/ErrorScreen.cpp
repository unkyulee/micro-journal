#include "ErrorScreen.h"
#include "app/app.h"

//
#include "display/CARDPUTER/display_CARDPUTER.h"


//
void ErrorScreen_setup()
{
    // when clearing background
    M5Cardputer.Display.fillRect(
        0,
        0,
        M5Cardputer.Display.width(),
        M5Cardputer.Display.height(),
        TFT_BLACK);
}

//
void ErrorScreen_render()
{
    // Text to be displayed
    JsonDocument &app = status();
    String text = app["error"].as<String>();

    ///
    M5Cardputer.Display.setTextSize(1); // Adjust text size as needed
    M5Cardputer.Display.setTextColor(TFT_WHITE, TFT_RED);
    M5Cardputer.Display.setFont(&fonts::Orbitron_Light_24);

    // Draw text in the middle of the screen
    drawWrappedText(text, 0, 0, M5Cardputer.Display.width());
}

//
void ErrorScreen_keyboard(char key)
{
    // Any key press reset ESP32
    _log("Restart ESP32\n");
    ESP.restart(); // Reset the ESP32
}