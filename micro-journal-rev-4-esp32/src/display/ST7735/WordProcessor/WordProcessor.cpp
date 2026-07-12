#include "WordProcessor.h"
#include "app/app.h"

//
#include "service/Editor/Editor.h"
#include "keyboard/keyboard.h"
#include "display/display.h"

//
#include "display/ST7735/display_ST7735.h"

#define FONT_SMALL u8g2_font_profont12_tf

//
int screen_width = 160;
int screen_height = 80;

// Generated Korean font - see script/generate_u8g2_korean_font.py. Same
// u8g2 binary format as the other builds (GalmuriMono11, SIL OFL),
// rasterized at its native 12px pixel size for this tiny 160x80 panel and
// the RP2040's limited flash: dual-width 6/12px Latin/Hangul, ~59KB.
extern const uint8_t u8g2_font_galmuri_12_t_korean[];

// Selectable word processor fonts. lineHeight is the u8g2 paragraph
// advance (what print("\n") moves by, = ascent_para - descent_para), not
// the glyph height. Declared glyph widths are corrected at setup by
// measuring the actual font. The Korean font renders Hangul full-width,
// hence the double-width charColumns rule.
static EditorFont WP_FONTS[] = {
    {"profont17", u8g2_font_profont17_tf, 9, 14, nullptr},
    {"korean12", u8g2_font_galmuri_12_t_korean, 6, 14, editorfont_hangul_columns},
};
static const int WP_FONT_COUNT = sizeof(WP_FONTS) / sizeof(WP_FONTS[0]);

// active font, selected in WP_setup from config "font"
static EditorFont *wp_font = &WP_FONTS[0];

// Lines will be rendered at the bottom on the screen
// need to calculate the Y position considering the status bar height
// layout derived from the active font metrics in WP_setup
static int editY = 72;
static int cursorY = 75;
const int cursorHeight = 2;

// Some flags
bool clear_background = true;
unsigned int last_sleep = millis();

//
void WP_setup(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    // setup default color
    JsonDocument &app = status();
    if (!app["config"]["foreground_color"].is<int>())
    {
        app["config"]["foreground_color"] = TFT_WHITE;
    }

    // pick the font from config ("font" is an index into WP_FONTS)
    int fontIndex = app["config"]["font"].as<int>();
    if (fontIndex < 0 || fontIndex >= WP_FONT_COUNT)
        fontIndex = 0;
    wp_font = &WP_FONTS[fontIndex];

    // measure the real glyph advance instead of trusting the declared value.
    // Measured as a difference: getUTF8Width() substitutes the LAST glyph's
    // tight bitmap width for its advance, so a single "M" under-reports on
    // fonts with trimmed glyph boxes - the difference cancels that and
    // leaves exactly one true advance.
    pu8f->setFont((const uint8_t *)wp_font->fontData);
    int16_t measured = pu8f->getUTF8Width("MM") - pu8f->getUTF8Width("M");
    if (measured > 0)
        wp_font->glyphWidth = measured;

    // derive the layout from the font metrics:
    // the edit line sits near the bottom of the screen and history lines
    // start at one line height (see WP_render_text)
    editY = screen_height - 8;
    cursorY = editY + 3;
    int rows = (editY - wp_font->lineHeight) / wp_font->lineHeight;
    int cols = screen_width / wp_font->glyphWidth;

    //
    _log("WP layout font: %s %dx%d cols: %d rows: %d editY: %d\n",
         wp_font->id, wp_font->glyphWidth, wp_font->lineHeight, cols, rows, editY);

    // Editor Init - setup screen size
    Editor::getInstance().init(cols, rows, wp_font);

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
    // the editor swapped to a different window of the file - force a full redraw
    if (Editor::getInstance().pageChanged)
    {
        Editor::getInstance().pageChanged = false;
        clear_background = true;
    }

    // timers
    WP_check_saved();
    WP_check_sleep();

    // CLEAR BACKGROUND
    WP_render_clear(ptft, pu8f);

    // RENDER TEXT
    WP_render_text(ptft, pu8f);

    // BLINK CURSOR
    WP_render_cursor(ptft, pu8f);

    // STATUS
    WP_render_status(ptft, pu8f);

    //
    if (clear_background)
        clear_background = false;

    // Editor House Keeping Tasks
    Editor::getInstance().loop();
}

//
void WP_render_text(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    JsonDocument &app = status();

    // LOAD COLORS
    uint16_t background_color = app["config"]["background_color"].as<uint16_t>();
    uint16_t foreground_color = app["config"]["foreground_color"].as<uint16_t>();

    // SET FONT
    pu8f->setFont((const uint8_t *)wp_font->fontData);
    pu8f->setForegroundColor(foreground_color);
    pu8f->setFontMode(1);

    // Cursor Information
    static int cursorLine_prev = 0;
    static int cursorLinePos_prev = 0;
    int cursorLine = Editor::getInstance().cursorLine;
    int cursorLinePos = Editor::getInstance().cursorLinePos;
    int totalLine = Editor::getInstance().totalLine;

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
        pu8f->setFont((const uint8_t *)wp_font->fontData);
        pu8f->setForegroundColor(foreground_color);
        pu8f->setFontMode(1);

        //
        pu8f->setCursor(0, wp_font->lineHeight);

        // start line
        int rows = Editor::getInstance().rows;

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
        _debug("Pushing sprite, cursorLine: %d, cursorLinePos: %d\n", cursorLine, cursorLinePos);

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
    char *line = Editor::getInstance().linePositions[line_num];
    int length = Editor::getInstance().lineLengths[line_num];

    //_debug("WP_render_line %d, length: %d\n", line_num, length);

    // render - the buffer holds UTF-8 and the u8g2 print pipeline decodes
    // multi-byte characters itself, so bytes stream straight through
    for (int i = 0; i < length; i++)
    {
        char value = *(line + i);
        if (value == '\n')
            continue;

        pu8f->print(value);
    }
}

//
// Render Cursor
void WP_render_cursor(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    JsonDocument &app = status();

    // retrieve color information
    uint16_t background_color = app["config"]["background_color"].as<uint16_t>();
    uint16_t foreground_color = app["config"]["foreground_color"].as<uint16_t>();

    // Cursor information
    static int cursorLineCols_prev = 0;
    int cursorLinePos = Editor::getInstance().cursorLinePos;
    int cursorLineCols = Editor::getInstance().cursorLineCols;
    int cursorPos = Editor::getInstance().cursorPos;

    // Calculate Cursor X position
    // reached the line where cursor is
    // cursor offset is measured in display columns so it stays correct
    // once double-width glyphs exist
    int cursorX = 0;
    if (Editor::getInstance().buffer[cursorPos - 1] != '\n' && cursorLinePos != 0)
    {
        cursorX = cursorLineCols * wp_font->glyphWidth;
    }

    // Blink the cursor every 500 ms
    static bool blink = false;
    static unsigned int last = millis();
    if (millis() - last > 500)
    {
        last = millis();
        blink = !blink;
    }

    // Delete previous cursor line
    if (cursorLineCols != cursorLineCols_prev)
    {
        //
        ptft->fillRect(cursorLineCols_prev * wp_font->glyphWidth, cursorY, wp_font->glyphWidth, cursorHeight, background_color);

        //
        cursorLineCols_prev = cursorLineCols;
    }

    // Cursor Blink will be always at the bottom of the screen
    if (blink)
        ptft->fillRect(cursorX, cursorY, wp_font->glyphWidth, cursorHeight, foreground_color);
    else
        ptft->fillRect(cursorX, cursorY, wp_font->glyphWidth, cursorHeight, background_color);
}

//
void WP_render_status(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    //
    JsonDocument &app = status();

    //
    const int width = 3;
    int color = TFT_RED;
    // SAVE STATUS
    if (Editor::getInstance().saved)
    {
        color = TFT_GREEN;
    }

    // file index number
    const int font_width = 12;
    int file_index = app["config"]["file_index"].as<int>();
    pu8f->setFont(FONT_SMALL);
    pu8f->setCursor(screen_width - font_width / 2, font_width * 2 / 3);
    pu8f->print(String(file_index));

    // Word count print
    int small_font_width = 10;
    int bufferSize = Editor::getInstance().wordCountFile + Editor::getInstance().wordCountBuffer; ;
    String bufferStr = String(bufferSize);
    // Calculate x position so it's right-aligned
    int textWidth = bufferStr.length() * small_font_width;
    int x = screen_width - textWidth - 2;     // 2px padding
    int y = screen_height; // near bottom of the screen

    // Clear background where buffer size will be drawn
    ptft->fillRect(x - 2, y - 2, textWidth + 4, small_font_width + 4, app["config"]["background_color"].as<uint16_t>());

    // Draw buffer size
    pu8f->setCursor(x, y);
    pu8f->print(bufferStr);

    // height 100% 80
    float batteryPercent = app["battery"].as<float>();
    int height = 80 * batteryPercent / 100.0;
    if (height < width)
        height = width;
    ptft->fillRect(screen_width - width, font_width, width, height - font_width, color);
}

//
// Clear Screen
// Do it as less as possible so that there is the least amount of the flicker
//
void WP_render_clear(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    // clear background
    if (clear_background)
    {
        // when clearing background
        // sprite will be activated to reduce the flicker
        return;
    }

    //
    JsonDocument &app = status();

    // the composing Hangul character was replaced in place - the new glyph
    // doesn't necessarily cover the old one's pixels, so repaint fully
    if (Editor::getInstance().charReplaced)
    {
        Editor::getInstance().charReplaced = false;
        clear_background = true;
    }

    // LOAD COLORS
    uint16_t background_color = app["config"]["background_color"].as<uint16_t>();
    uint16_t foreground_color = app["config"]["foreground_color"].as<uint16_t>();

    //
    static int cursorLine_prev = 0;
    static int cursorPos_prev = 0;
    int cursorLine = Editor::getInstance().cursorLine;
    int cursorPos = Editor::getInstance().cursorPos;
    int cursorLinePos = Editor::getInstance().cursorLinePos;
    int cursorLineLength = Editor::getInstance().lineLengths[cursorLine];

    //
    static int bufferSize_prev = 0;
    int bufferSize = Editor::getInstance().getBufferSize();

    // When new line clear everything
    if (cursorLine_prev != cursorLine)
    {
        //
        clear_background = true;

        //
        cursorLine_prev = cursorLine;
    }

    // When Backspace, trailing characters should be deleted
    // if it is backspace or del key
    if (cursorPos_prev >= cursorPos && bufferSize_prev != bufferSize)
        clear_background = true;

    if (cursorPos_prev != cursorPos)
    {
        // if it is typing at the end don't flicker
        if (cursorPos != bufferSize)
        {
            // if the edit line is empty then don't flicker
            //
            if (cursorLinePos + 1 != cursorLineLength)
                clear_background = true;
        }

        //
        cursorPos_prev = cursorPos;
    }

    if (bufferSize != bufferSize_prev)
    {
        bufferSize_prev = bufferSize;
    }
}

//
//
void WP_keyboard(int key, bool pressed)
{
    // ignore non pritable keys
    if (key == 0)
        return;

    JsonDocument &app = status();
    _debug("WP_keyboard key: %d, pressed: %d\n", key, pressed);

    // Check if menu key or ESC is pressed
    if (key == MENU)
    {
        if (!pressed)
        {
            _debug("Menu key pressed\n");
            // Save before transitioning to the menu
            Editor::getInstance().saveFile();

            if (app["knobLongPressed"].as<bool>())
            {
                // open menu
                _debug("WP_keyboard - Received MENU Key\n");
                app["screen"] = MENUSCREEN;
            }
            else
            {
                // move to writerDeck
                _debug("WP_keyboard - Received LONG PRESS MENU Key\n");
                app["screen"] = KEYBOARDSCREEN;

                // save the default screen to config
                app["config"]["UsbKeyboard"] = true;
                config_save();
            }
        }

        // menu button is ignored
        return;
    }

    if (key == 27)
    {
        if (!pressed)
        {
            // Save before transitioning to the menu
            Editor::getInstance().saveFile();

            // open menu
            _debug("WP_keyboard - Received ESC Key\n");
            app["screen"] = MENUSCREEN;
        }

        // ESC button is ignored
        return;
    }

    // Check if File Change request is pressed
    if (key >= 1000 && key <= 1010)
    {
        if (!pressed)
        {
            int fileIndex = key - 1000;
            _log("File Change Requested: %d\n", fileIndex);

            //
            Editor::getInstance().saveFile();

            // save config
            app["config"]["file_index"] = fileIndex;
            config_save();

            // load new file
            Editor::getInstance().loadFile(format("/%d.txt", fileIndex));
        }
    }

    else
    {
        // send the keys to the editor
        Editor::getInstance().keyboard(key, pressed);
    }
}

//
// Check if text is saved
void WP_check_saved()
{
    //
    static unsigned int last = millis();
    static int lastBufferSize = Editor::getInstance().getBufferSize();
    int bufferSize = Editor::getInstance().getBufferSize();

    //
    // when the file is saved then extend the autosave timer
    if (lastBufferSize != bufferSize)
    {
        last = millis();

        //
        lastBufferSize = bufferSize;
    }

    //
    // when idle for 3 seconds then auto save
    if (millis() - last > 3000)
    {
        //
        last = millis();

        if (!Editor::getInstance().saved)
            Editor::getInstance().saveFile();
    }
}

//
//
void WP_check_sleep()
{
    //
    // when the file is saved then extend the sleep timer
    if (!Editor::getInstance().saved)
    {
        // when typed then reset sleep timer
        last_sleep = millis();
    }

    // 600 seconds - 10 minutes
    if (millis() - last_sleep > 600000)
    {
        // if no action for 10 minute go to sleep
        last_sleep = millis();

        //
        JsonDocument &app = status();
        app["screen"] = SLEEPSCREEN;
    }
}
