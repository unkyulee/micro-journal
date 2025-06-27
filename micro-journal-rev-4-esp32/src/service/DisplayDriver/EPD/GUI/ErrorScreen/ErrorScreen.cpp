#include "ErrorScreen.h"
#include "../../display_EPD.h"

//
#include "app/app.h"

//
void ErrorScreen_setup()
{
    app_log("Error Screen Setup\n");
}

//
void ErrorScreen_render()
{
    // Text to be displayed
    JsonDocument &app = app_status();
    String text = app["error"].as<String>();

    // Turn on the display
    epd_poweron();

    // Draw text
    int32_t x = 18;
    int32_t y = 50;
    write_string(display_EPD_font(), text.c_str(), &x, &y, NULL);

    // Turn off the display
    // epd_poweroff_all();
    epd_poweroff();
}

//
void ErrorScreen_keyboard(char key)
{
    // Any key press reset ESP32
    ESP.restart(); // Reset the ESP32
    delay(1000);
}