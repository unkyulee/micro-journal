#include "Brightness.h"
#include "../Menu.h"
#include "app/app.h"
#include "display/display.h"

//
#include "display/CARDPUTER/display_CARDPUTER.h"

//
void Brightness_setup()
{
    //
    Menu_clear();
}

//
void Brightness_render()
{
    JsonDocument &app = status();
    int brightness = app["config"]["brightness"].as<int>();

    //
    M5Cardputer.Display.setTextSize(0.5);
    M5Cardputer.Display.setFont(&fonts::Orbitron_Light_24);
    M5Cardputer.Display.setTextColor(TFT_WHITE, TFT_BLACK);

    M5Cardputer.Display.drawString("BRIGHTNESS", 5, 15);
    
    // --- Progress Bar ---
    int barX = 20;
    int barY = 50;
    int barWidth = M5Cardputer.Display.width() - 50;
    int barHeight = 15;

    // Bar background (empty)
    M5Cardputer.Display.fillRect(barX, barY, barWidth, barHeight, TFT_DARKGREY);

    // Scale brightness smoothly (0–10 → 0–100%)
    float progress = (float)brightness / 10.0f; // 0.0 → 1.0

    int fillWidth = (int)(barWidth * progress);

    // Draw filled portion
    M5Cardputer.Display.fillRect(barX, barY, fillWidth, barHeight, TFT_WHITE);

    // Optional: Draw border
    M5Cardputer.Display.drawRect(barX, barY, barWidth, barHeight, TFT_WHITE);
}

//
void Brightness_keyboard(char key)
{
    //
    _debug("Brightness_Keyboard %d\n", key);
    JsonDocument &app = status();

    // MENU - SELECTED ACTION
    if (key == MENU || key == '\n')
    {
        // Go back to Home
        _log("Exit Brightness");

        //
        app["menu"]["state"] = MENU_HOME;

        // Save Config
        config_save();
    }

    // UP
    else if (key == 20 || key == ';')
    {
        int brightness = app["config"]["brightness"].as<int>();
        brightness++;
        if (brightness > 10)
            brightness = 10;

        app["config"]["brightness"] = brightness;
    }

    // DOWN
    else if (key == 21 || key == '.')
    {
        int brightness = app["config"]["brightness"].as<int>();
        brightness--;
        if (brightness < 0)
            brightness = 0;

        app["config"]["brightness"] = brightness;
    }
}
