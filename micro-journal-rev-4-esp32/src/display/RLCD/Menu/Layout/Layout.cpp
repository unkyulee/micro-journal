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
        "[H] Korean",
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

    if (key >= 'A' && key <= 'Z')
        key = key - 'A' + 'a';

    // Map keys to keyboard layouts
    struct LayoutOption
    {
        const char *layout;
        int font;
    };

    static const std::unordered_map<char, LayoutOption> layout_map = {
        {'a', {"INT", 0}}, // International
        {'b', {"BE", 0}},  // Belgian
        {'c', {"CA", 0}},  // Canadian
        {'d', {"DV", 0}},  // Dvorak
        {'f', {"FR", 0}},  // French
        {'g', {"GE", 0}},  // German
        {'h', {"KR", 2}},  // Korean
        {'i', {"IT", 0}},  // Italian
        {'k', {"UK", 0}},  // UK
        {'l', {"LAT", 0}}, // Latin America
        {'s', {"SWE", 0}}, // Swedish
        {'u', {"US", 0}}   // US (default)
    };

    // Find the layout based on the key
    auto it = layout_map.find(key);
    if (it != layout_map.end())
    {
        app["config"]["keyboard_layout"] = it->second.layout;
        app["config"]["font"] = it->second.font;
    }
    else
    {
        // Log invalid key
        _log("Invalid key pressed. Defaulting to US layout. %c\n", key);
        app["config"]["keyboard_layout"] = "US";
        app["config"]["font"] = 0;
    }

    // Save configuration and update the screen
    config_save();
    app["screen"] = WORDPROCESSOR;

    // Go back to the home menu
    app["menu"]["state"] = MENU_HOME;
}
