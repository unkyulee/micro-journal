#include "Language.h"
#include "../Menu.h"
#include "app/app.h"
#include "display/display.h"

#include "display/CARDPUTER/display_CARDPUTER.h"

static const char *language_options[] = {
    "International",
    "Belgian",
    "Canadian",
    "Dvorak",
    "French",
    "German",
    "Italian",
    "UK",
    "Latin America",
    "Swedish",
    "US"};

static const char *language_codes[] = {
    "INT",
    "BE",
    "CA",
    "DV",
    "FR",
    "GE",
    "IT",
    "UK",
    "LAT",
    "SWE",
    "US"};

static int language_selected = 0;

static int Language_find_index(const String &code)
{
    for (int i = 0; i < 11; i++)
    {
        if (code == language_codes[i])
            return i;
    }
    return 10;
}

void Language_setup()
{
    Menu_clear();

    JsonDocument &app = status();
    String layout = app["config"]["keyboard_layout"].as<String>();
    if (layout == "null" || layout.isEmpty())
        layout = "US";
    language_selected = Language_find_index(layout);
}

void Language_render()
{
    M5Cardputer.Display.setTextSize(0.6);
    M5Cardputer.Display.setFont(&fonts::Orbitron_Light_24);

    M5Cardputer.Display.setTextColor(TFT_WHITE, TFT_BLACK);
    M5Cardputer.Display.drawString("LANGUAGE", 4, 0);

    const int visible_lines = 6;
    const int center_index = visible_lines / 2;
    const int menu_length = sizeof(language_options) / sizeof(language_options[0]);
    const int line_height = 18;
    const int start_y = 18;

    int start = language_selected - center_index;
    if (start < 0)
        start = 0;
    if (start > menu_length - visible_lines)
        start = menu_length - visible_lines;
    if (start < 0)
        start = 0;

    int y = start_y;
    for (int i = 0; i < visible_lines; i++)
    {
        int menu_idx = start + i;
        if (menu_idx >= 0 && menu_idx < menu_length)
        {
            if (menu_idx == language_selected)
                M5Cardputer.Display.setTextColor(TFT_WHITE, TFT_RED);
            else
                M5Cardputer.Display.setTextColor(TFT_WHITE, TFT_BLACK);

            M5Cardputer.Display.drawString(language_options[menu_idx], 6, y);
            y += line_height;
        }
    }
}

void Language_keyboard(char key)
{
    JsonDocument &app = status();

    // DOWN
    if (key == 21 || key == '.')
    {
        language_selected++;
        if (language_selected >= 11)
            language_selected = 0;
    }

    // UP
    else if (key == 20 || key == ';')
    {
        language_selected--;
        if (language_selected < 0)
            language_selected = 10;
    }

    // ENTER
    else if (key == '\n' || key == MENU)
    {
        app["config"]["keyboard_layout"] = language_codes[language_selected];
        config_save();
        app["menu"]["state"] = MENU_HOME;
    }

    // ESC
    else if (key == 27)
    {
        app["menu"]["state"] = MENU_HOME;
    }
}
