#include "ErrorScreen.h"
#include "app/app.h"

// Whether the last ErrorScreen_render() actually changed anything visible - the
// caller uses this to decide whether the (expensive, full 30KB SPI) panel
// refresh is worth doing this tick.
static bool needsDisplay = true;

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

    // decide whether the panel actually needs to be pushed over SPI this tick
    static String text_prev = "";
    needsDisplay = (text != text_prev);
    text_prev = text;
}

bool ErrorScreen_needsDisplay()
{
    return needsDisplay;
}

//
void ErrorScreen_keyboard(char key)
{
    // Any key press reset ESP32
    _log("Restarting ESP32\n");
    ESP.restart(); // Reset the ESP32
}