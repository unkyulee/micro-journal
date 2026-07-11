#include "Layout.h"
#include "../Menu.h"
#include "app/app.h"
#include "display/display.h"
#include "../../WordProcessor/WordProcessor.h"

#include <unordered_map>
#include <functional>
#include <utility>

void Layout_setup(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    // Clear the screen
    Menu_clear();
    ptft->fillScreen(TFT_BLACK);
}

void Layout_render(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    JsonDocument &app = status();

    ptft->setTextColor(TFT_WHITE, TFT_BLACK);

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
    const int count = sizeof(layout_options) / sizeof(layout_options[0]);

    // Two columns so the list fits the screen height as more layouts are
    // added - a single column of 12 entries crowds the 240px panel.
    const int rowsPerCol = (count + 1) / 2;
    const int lineHeight = 18;
    const int col2X = 165;

    for (int i = 0; i < count; i++)
    {
        int col = i / rowsPerCol;
        int row = i % rowsPerCol;
        ptft->setCursor(col == 0 ? 0 : col2X, 30 + row * lineHeight, 2);
        ptft->print(layout_options[i]);
    }
}

void Layout_keyboard(char key)
{
    JsonDocument &app = status();

    if (key >= 'A' && key <= 'Z')
        key = key - 'A' + 'a';

    struct LayoutOption
    {
        const char *layout;
        int font; // index into WP_FONTS (WordProcessor.cpp)
    };

    // Map keys to keyboard layouts
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
        Serial.println("Invalid key pressed. Defaulting to US layout.");
        app["config"]["keyboard_layout"] = "US";
        app["config"]["font"] = 0;
    }

    // Save configuration and update the screen
    config_save();
    app["screen"] = WORDPROCESSOR;

    // Go back to the home menu
    app["menu"]["state"] = MENU_HOME;
}
