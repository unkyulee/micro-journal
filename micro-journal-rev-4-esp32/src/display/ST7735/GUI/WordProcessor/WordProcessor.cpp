#include "WordProcessor.h"
#include "app/app.h"

//
#include "service/Editor/Editor.h"
#include "keyboard/keyboard.h"
#include "display/display.h"

//
#include "display/ST7735/display_ST7735.h"

//
int STATUSBAR_Y = 140;
int screen_width = 160;
int screen_height = 80;
bool clear_background = true;
unsigned int last_sleep = millis();

//
const int font_width = 12;
const int font_height = 18;
const int cursorY = 50;
const int cursorHeight = 2;
//
const int editY = cursorY - 4;

//
void WP_setup(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    // editor instantiate
    Editor &editor = Editor::getInstance();
    editor.screenBuffer.rows = 4;
    editor.screenBuffer.cols = 20;

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
    // timers
    WP_check_saved();
    WP_check_sleep();

    // CLEAR BACKGROUND
    WP_render_clear(ptft, pu8f);

    // RENDER TEXT
    WP_render_text(ptft, pu8f);

    // RENDER STATUS BAR
    WP_render_status(ptft, pu8f);

    // BLINK CURSOR
    WP_render_cursor(ptft, pu8f);

    //
    if (clear_background)
        clear_background = false;
}

//
void WP_render_text(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    JsonDocument &app = status();

    // LOAD COLORS
    uint16_t background_color = app["config"]["background_color"].as<uint16_t>();
    uint16_t foreground_color = app["config"]["foreground_color"].as<uint16_t>();

    // SET FONT
    pu8f->setFont(u8g2_font_profont15_tf);
    pu8f->setForegroundColor(foreground_color);
    pu8f->setFontMode(1);

    // Cursor Information
    static int cursorLine_prev = 0;
    static int cursorLinePos_prev = 0;
    int cursorLine = Editor::getInstance().screenBuffer.cursorLine;
    int cursorLinePos = Editor::getInstance().screenBuffer.cursorLinePos;
    int totalLine = Editor::getInstance().screenBuffer.total_line;

    //
    // Middle part of the text will be rendered
    // Only when refresh background is called
    //
    // initiate sprite
    TFT_eSprite sprite = display_ST7735_sprite();
    if (clear_background)
    {
        sprite.createSprite(screen_width, screen_height);
        sprite.fillSprite(background_color);

        //
        pu8f->begin(sprite);
        pu8f->setFont(u8g2_font_profont15_mf);
        pu8f->setForegroundColor(foreground_color);
        pu8f->setFontMode(1);

        //
        pu8f->setCursor(0, font_height + font_height / 2);

        // start line
        int rows = Editor::getInstance().screenBuffer.rows;

        //
        for (int i = cursorLine - rows; i < cursorLine; i++)
        {
            if (i >= 0)
                WP_render_line(ptft, pu8f, i);

            //
            pu8f->print("\n");
        }
    }

    //
    // Bottom line will the the edit area
    //
    pu8f->setCursor(0, editY);
    WP_render_line(ptft, pu8f, cursorLine);

    // Clean up sprite
    if (clear_background)
    {
        // push sprite
        sprite.pushSprite(0, 0);
        sprite.deleteSprite();
        pu8f->begin(*ptft);
    }
}

//
//
void WP_render_line(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f, int line_num)
{
    char *line = Editor::getInstance().screenBuffer.line_position[line_num];
    int length = Editor::getInstance().screenBuffer.line_length[line_num];

    //_debug("WP_render_line %d, length: %d\n", line_num, length);

    // render
    for (int i = 0; i < length; i++)
    {
        // convert extended ascii into a streamlined string
        uint8_t value = *(line + i);
        if (value == '\n')
            continue;

        String str = asciiToUnicode(value);
        if (str.length() == 0)
            pu8f->print((char)value);
        else
            pu8f->print(str);
    }
}

//
// Check if text is saved
void WP_check_saved()
{
}

//
//
void WP_check_sleep()
{
}

//
//
void WP_render_status(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
}

//
// Render Cursor
void WP_render_cursor(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
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
//
void WP_keyboard(int key)
{
    _debug("WP_keyboard %c\n", key);

    // Check if menu key is pressed
    if (key == MENU)
    {
        // Save before transitioning to the menu
        Editor::getInstance().saveFile();

        //
        JsonDocument &app = status();
        app["screen"] = MENUSCREEN;
    }

    else
    {
        // send the keys to the editor
        Editor::getInstance().keyboard(key);
    }
}
