#include "Language.h"
#include "../Menu.h"
#include "app/app.h"
#include "display/display.h"

// two options, matching the entries below: 0 = US, 1 = KR
static int selectedLang = 0;

// {layout code, WP_FONTS index}. The built-in TFT font used for this menu
// is ASCII-only, so the labels are romanized rather than "한국어".
struct LangOption
{
    const char *label;
    const char *layout;
    int font;
};

static const LangOption langs[] = {
    {"English", "US", 0},
    {"Korean", "KR", 1},
};
static const int langCount = sizeof(langs) / sizeof(langs[0]);

//
void Language_setup(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    Menu_clear();

    // start on the currently configured language
    JsonDocument &app = status();
    String locale = app["config"]["keyboard_layout"].as<String>();
    selectedLang = (locale == "KR") ? 1 : 0;
}

//
void Language_render(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    ptft->setCursor(0, 0);
    ptft->setTextSize(2);

    ptft->setTextColor(TFT_WHITE, TFT_BLACK);
    ptft->println("");
    ptft->println("Language");
    ptft->println("");

    for (int i = 0; i < langCount; i++)
    {
        if (i == selectedLang)
            ptft->setTextColor(TFT_WHITE, TFT_RED);
        else
            ptft->setTextColor(TFT_WHITE, TFT_BLACK);

        ptft->print("  ");
        ptft->println(langs[i].label);
    }
}

//
void Language_keyboard(char key, bool pressed)
{
    JsonDocument &app = status();

    // UP / DOWN toggle between the two options
    if (key == 20 || key == 21)
    {
        selectedLang = (selectedLang + 1) % langCount;
    }

    // MENU / ENTER applies the selection
    else if (key == MENU || key == '\n')
    {
        app["config"]["keyboard_layout"] = langs[selectedLang].layout;
        app["config"]["font"] = langs[selectedLang].font;
        config_save();

        // load USB Keyboard
        app["screen"] = WORDPROCESSOR;
    }
}
