#include "Sleep.h"
#include "../../display_EPD.h"
#include "editor/editor.h"

//
#include "app/app.h"
#include "display/display.h"
#include "keyboard/keyboard.h"

bool sleep_rendered = false;
//
void Sleep_setup()
{
    //
    app_log("Sleep Screen Setup\n");

    // render just once
    sleep_rendered = false;
}

//
void Sleep_render()
{
    //
    if (sleep_rendered)
        return;

    //
    sleep_rendered = true;

    // SAVE FILE
    Editor::getInstance().saveFile();

    // Turn on the display
    epd_poweron();

    // Clear Screen
    epd_clear();

    // RENDER TEXT
    int32_t x = 250;
    int32_t y = 200;
    write_string(display_EPD_font(), "Micro Journal Rev.7", &x, &y, NULL);
    y += 50;
    x = 300;
    write_string(display_EPD_font(), "by Un Kyu Lee", &x, &y, NULL);

    // Turn off the display
    Serial.end();
    epd_poweroff_all();

    // BOOT(STR_IO0) Button wakeup
    esp_sleep_enable_ext1_wakeup(_BV(GPIO_NUM_21), ESP_EXT1_WAKEUP_ANY_LOW);
    esp_deep_sleep_start();
}

//
void Sleep_keyboard(char key)
{
    // Any Keystroke moves to word processor
    JsonDocument &app = app_status();
    app["screen"] = WORDPROCESSOR;
}
