#include "ErrorScreen.h"
#include "../display_EPD.h"

//
#include "app/app.h"

//
void ErrorScreen_setup()
{
    _log("Error Screen Setup\n");

    // Clear Screen
    epd_poweron();
    epd_clear_quick(epd_full_screen(), 4, 50);

    // Text to be displayed
    JsonDocument &app = status();
    String text = app["error"].as<String>();

    // Draw text
    int32_t x = 18;
    int32_t y = 50;
    write_string(display_EPD_font(), text.c_str(), &x, &y, NULL);

    // Turn off the display
    // epd_poweroff_all();
    epd_poweroff();
}

//
void ErrorScreen_render()
{
}

//
void ErrorScreen_keyboard(char key)
{
    // Any key press reset ESP32
    _log("Restartinhg ESP32\n");
    ESP.restart(); // Reset the ESP32
}