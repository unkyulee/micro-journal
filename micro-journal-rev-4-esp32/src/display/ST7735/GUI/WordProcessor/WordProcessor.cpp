#include "WordProcessor.h"
#include "app/app.h"
#include "editor/editor.h"

// 
int STATUSBAR_Y = 224;
int screen_width = 320;
int screen_height = 240;
bool clear_background = true;
unsigned int last_sleep = millis();

//
const int font_width = 12;
const int font_height = 18;
const int cursorY = 210;
const int cursorHeight = 2;
//
const int editY = cursorY - 4;

//
void WP_setup(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    // editor instantiate
    Editor::getInstance();

    // setup default color
    JsonDocument &app = status();
    if (!app["config"]["foreground_color"].is<int>())
    {
        app["config"]["foreground_color"] = TFT_WHITE;
    }

    // load file from the editor
    int file_index = app["config"]["file_index"].as<int>();
    Editor::getInstance().loadFile(format("/%d.txt", file_index));

    // start from clear background
    clear_background = true;

    // sleep timer reset
    last_sleep = millis();
}

//
void WP_render(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
}

// Check if text is saved
void WP_check_saved()
{
}

void WP_check_sleep()
{
}

//
// Clear Screen
// Do it as less as possible so that there is the least amount of the flicker
//
void WP_render_clear(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
}

//
void WP_render_line(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f, int line_num)
{
}

//
void WP_render_text(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
}

void WP_render_status(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
}

//
// Render Cursor
void WP_render_cursor(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
}

//
void WP_keyboard(char key)
{
}
