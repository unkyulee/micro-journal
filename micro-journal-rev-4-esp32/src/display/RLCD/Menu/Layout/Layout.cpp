#include "Layout.h"
#include "../Menu.h"
#include "app/app.h"
#include "display/display.h"
#include "../../WordProcessor/WordProcessor.h"

#include <unordered_map>
#include <functional>
#include <utility>

void Layout_setup(ST7305_4p2_BW_DisplayDriver *display, U8G2_FOR_ST73XX *u8)
{
    // Clear the screen
    Menu_clear();
    display->clearDisplay();
}

void Layout_render(ST7305_4p2_BW_DisplayDriver *display, U8G2_FOR_ST73XX *u8)
{
    JsonDocument &app = status();
    
    u8->setCursor(0, 50);

    // Define layout options
    static const char *layout_options[] = {
        "[A] International",
        "[B] Belgian",
        "[C] Canadian",
        "[D] Dvorak",
        "[F] French",
        "[G] German",
        "[I] Italian",
        "[K] UK",
        "[L] Latin America",
        "[S] Swedish",
        "[U] US"
    };

    // Print each layout option
    for (const char *option : layout_options)
    {
        u8->println(option);
    }
}

void Layout_keyboard(char key)
{
    JsonDocument &app = status();

    // Map keys to keyboard layouts
    static const std::unordered_map<char, const char *> layout_map = {
        {'a', "INT"}, // International
        {'b', "BE"},  // Belgian
        {'c', "CA"},  // Canadian
        {'d', "DV"},  // Dvorak
        {'f', "FR"},  // French
        {'g', "GE"},  // German
        {'i', "IT"},  // Italian
        {'k', "UK"},  // UK
        {'l', "LAT"}, // Latin America
        {'s', "SWE"}, // Swedish
        {'u', "US"}   // US (default)
    };

    // Find the layout based on the key
    auto it = layout_map.find(key);
    if (it != layout_map.end())
    {
        app["config"]["keyboard_layout"] = it->second;
    }
    else
    {
        // Log invalid key
        Serial.println("Invalid key pressed. Defaulting to US layout.");
        app["config"]["keyboard_layout"] = "US";
    }

    // Save configuration and update the screen
    config_save();
    app["screen"] = WORDPROCESSOR;

    // Go back to the home menu
    app["menu"]["state"] = MENU_HOME;
}
