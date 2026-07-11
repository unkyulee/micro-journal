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

    const char *layouts[] = {
        "[a] International",
        "[b] Belgian",
        "[c] Canadian",
        "[d] Dvorak",
        "[f] French",
        "[n] Finnish",
        "[g] German",
        "[h] Korean",
        "[i] Italian",
        "[w] Norwegian",
        "[k] UK",
        "[u] US"
    };
    const int count = sizeof(layouts) / sizeof(layouts[0]);

    // Two columns so the list fits the screen height as more layouts are
    // added - a single column of 12+ entries runs into the status bar.
    const int rowsPerCol = (count + 1) / 2;
    const int lineHeight = 40;
    const int col2X = 500;

    for (int i = 0; i < count; i++)
    {
        int col = i / rowsPerCol;
        int row = i % rowsPerCol;
        int cursorX = col == 0 ? 10 : col2X;
        int cursorY = 100 + row * lineHeight;
        writeln((GFXfont *)&systemFont, layouts[i], &cursorX, &cursorY, display_EPD_framebuffer());
    }
}

//
void Layout_keyboard(char key)
{
    JsonDocument &app = status();

    struct LayoutOption
    {
        const char *layout;
        int font; // index into WP_FONTS (WordProcessor.cpp)
    };

    // Define a map for key-to-layout mappings
    static const std::map<char, LayoutOption> layoutMap = {
        {'a', {"INT", 0}},
        {'b', {"BE", 0}},
        {'c', {"CA", 0}},
        {'d', {"DV", 0}},
        {'f', {"FR", 0}},
        {'n', {"FN", 0}},
        {'g', {"GE", 0}},
        {'h', {"KR", 1}}, // Korean
        {'i', {"IT", 0}},
        {'w', {"NO", 0}},
        {'k', {"UK", 0}},
        {'u', {"US", 0}} // Default US layout
    };

    // Find the layout corresponding to the key
    LayoutOption selected = {"US", 0}; // Default to US layout
    auto it = layoutMap.find(key);
    if (it != layoutMap.end())
    {
        selected = it->second;
    }

    //
    app["config"]["keyboard_layout"] = selected.layout;
    app["config"]["font"] = selected.font;
    config_save();

    //
    app["screen"] = WORDPROCESSOR;
    app["menu"]["state"] = MENU_HOME;
}
