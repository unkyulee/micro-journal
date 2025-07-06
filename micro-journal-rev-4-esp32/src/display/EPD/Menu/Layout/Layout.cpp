#include "Layout.h"
#include "../Menu.h"
#include "app/app.h"
#include "display/display.h"
#include "keyboard/keyboard.h"
#include <display/EPD/display_EPD.h>
#include <map> // Include map for key-to-layout mapping

//
void Layout_setup()
{
    // when entering the screen
    // clear the screen
    Menu_clear();
}

//
void Layout_render()
{
    JsonDocument &app = status();

    // Choose a keyboard layout
    int cursorX = 10;
    int cursorY = 60;

    const char *layouts[] = {
        "[a] International",
        "[b] Belgian",
        "[c] Canadian",
        "[d] Dvorak",
        "[f] French",
        "[g] German",
        "[i] Italian",
        "[k] UK",
        "[n] Finnish",
        "[u] US"
    };

    for (int i = 0; i < sizeof(layouts) / sizeof(layouts[0]); ++i)
    {
        cursorX = 10;
        cursorY += 40;
        writeln((GFXfont *)&systemFont, layouts[i], &cursorX, &cursorY, display_EPD_framebuffer());
    }
}

//
void Layout_keyboard(char key)
{
    JsonDocument &app = status();

    // Define a map for key-to-layout mappings
    static const std::map<char, const char *> layoutMap = {
        {'a', "INT"},
        {'b', "BE"},
        {'c', "CA"},
        {'d', "DV"},
        {'f', "FR"},
        {'g', "GE"},
        {'i', "IT"},
        {'k', "UK"},
        {'n', "FN"},
        {'u', "US"} // Default US layout
    };

    // Find the layout corresponding to the key
    const char *selectedLayout = "US"; // Default to US layout
    auto it = layoutMap.find(key);
    if (it != layoutMap.end())
    {
        selectedLayout = it->second;
    }

    //
    app["config"]["keyboard_layout"] = selectedLayout;
    config_save();

    //
    app["screen"] = WORDPROCESSOR;
    app["menu"]["state"] = MENU_HOME;
}
