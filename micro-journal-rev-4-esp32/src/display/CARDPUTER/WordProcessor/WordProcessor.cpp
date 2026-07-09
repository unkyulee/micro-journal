#include "WordProcessor.h"
#include "app/app.h"

//
#include "service/Editor/Editor.h"
#include "keyboard/keyboard.h"
#include "display/display.h"

//
#include "display/CARDPUTER/display_CARDPUTER.h"
#include <u8g2_fonts.h>

extern const uint8_t u8g2_font_profont22_tf[];
extern const uint8_t u8g2_font_profont29_tf[];

//
int screen_width = 240;
int screen_height = 135;

static const lgfx::U8g2font g_profont22(u8g2_font_profont22_tf);
static const lgfx::U8g2font g_profont29(u8g2_font_profont29_tf);

// Selectable word processor fonts. This backend advances the pen manually
// by glyphWidth when drawing (see WP_render_line), so glyphWidth here is
// the rendering grid (glyph width plus letter spacing), not the font's own
// advance - it is used as declared and never re-measured.
static EditorFont WP_FONTS[] = {
    {"profont22", &g_profont22, 14, 22, nullptr},
    {"profont29", &g_profont29, 18, 29, nullptr},
};
static const int WP_FONT_COUNT = sizeof(WP_FONTS) / sizeof(WP_FONTS[0]);

// active font, selected in WP_setup from config "font"
static EditorFont *wp_font = &WP_FONTS[0];

// Lines will be rendered at the bottom on the screen
// need to calculate the Y position considering the status bar height
// layout derived from the active font metrics in WP_setup
static int editY = 90;
static int cursorY = 110;
const int cursorHeight = 2;

// Some flags
bool clear_background = true;
unsigned int last_sleep = millis();

//
void WP_setup()
{
    // setup default color
    JsonDocument &app = status();

    // pick the font from config ("font" is an index into WP_FONTS)
    int fontIndex = app["config"]["font"].as<int>();
    if (fontIndex < 0 || fontIndex >= WP_FONT_COUNT)
        fontIndex = 0;
    wp_font = &WP_FONTS[fontIndex];

    // derive the layout from the font metrics:
    // the status bar occupies the bottom 18 pixels, the edit line sits
    // right above it and the history lines fill the rest
    int statusbarY = screen_height - 18;
    int rows = (statusbarY - 2 - wp_font->lineHeight) / wp_font->lineHeight;
    int cols = screen_width / wp_font->glyphWidth;
    editY = rows * wp_font->lineHeight + 2;
    cursorY = editY + wp_font->lineHeight - 2;

    //
    _log("WP layout font: %s %dx%d cols: %d rows: %d editY: %d\n",
         wp_font->id, wp_font->glyphWidth, wp_font->lineHeight, cols, rows, editY);

    // Editor Init - setup screen size
    Editor::getInstance().init(cols, rows, wp_font);

    // load file from the editor
    int file_index = app["config"]["file_index"].as<int>();
    String filename = format("/%d.txt", file_index);
    _log("[WP_setup] load file [%s]\n", filename.c_str());
    Editor::getInstance().loadFile(filename);

    //
    if (!app["config"]["foreground_color"].is<int>())
    {
        app["config"]["foreground_color"] = TFT_WHITE;
    }

    // start from clear background
    clear_background = true;

    // sleep timer reset
    last_sleep = millis();
}

//
void WP_render()
{
    // the editor swapped to a different window of the file - force a full redraw
    if (Editor::getInstance().pageChanged)
    {
        Editor::getInstance().pageChanged = false;
        clear_background = true;
    }

    // timers
    WP_check_saved();

    // CLEAR BACKGROUND
    WP_render_clear();

    // RENDER TEXT
    WP_render_text();

    // BLINK CURSOR
    WP_render_cursor();

    // STATUS
    WP_render_status();

    if (clear_background)
        clear_background = false;

    // Editor House Keeping Tasks
    Editor::getInstance().loop();
}

//
void WP_render_text()
{
    JsonDocument &app = status();

    // LOAD COLORS
    uint16_t background_color = app["config"]["background_color"].as<uint16_t>();
    uint16_t foreground_color = app["config"]["foreground_color"].as<uint16_t>();

    // SET FONT
    M5Cardputer.Display.setTextSize(1);
    M5Cardputer.Display.setTextColor(foreground_color, background_color);
    M5Cardputer.Display.setFont((const lgfx::IFont *)wp_font->fontData);

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
    if (clear_background)
    {
        // start line
        int rows = Editor::getInstance().rows;

        //
        int y = 0;
        for (int i = cursorLine - rows; i < cursorLine; i++)
        {
            if (i >= 0)
                WP_render_line(i, y);

            // new line
            y += wp_font->lineHeight;
        }
    }
    else
    {
        //
        // Bottom line will the the edit area
        //
        WP_render_line(cursorLine, editY);
    }
}

//
//
void WP_render_line(int line_num, int y)
{
    char *line = Editor::getInstance().linePositions[line_num];
    int length = Editor::getInstance().lineLengths[line_num];

    // render - this backend places every character on a fixed grid, so
    // walk the UTF-8 buffer one whole character at a time
    int x = 0;
    int i = 0;
    while (i < length)
    {
        const char *p = line + i;
        if (*p == '\n')
        {
            i++;
            continue;
        }

        int charLen;
        uint32_t codepoint = utf8_decode(p, &charLen);

        if (charLen == 1)
            M5Cardputer.Display.drawChar(*p, x, y + wp_font->lineHeight - 4);
        else
        {
            // hand the whole multi-byte character to the UTF-8-aware
            // string renderer
            char utf8Char[5];
            memcpy(utf8Char, p, charLen);
            utf8Char[charLen] = '\0';
            M5Cardputer.Display.drawString(utf8Char, x, y);
        }

        // advance the pen on the fixed rendering grid by the character's
        // display width (double-width glyphs take two cells)
        x += Editor::getInstance().charColumns(codepoint) * wp_font->glyphWidth;
        i += charLen;
    }
}

//
// Render Cursor
void WP_render_cursor()
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
        M5Cardputer.Display.fillRect(cursorLineCols_prev * wp_font->glyphWidth, cursorY, wp_font->glyphWidth, cursorHeight, background_color);

        //
        cursorLineCols_prev = cursorLineCols;
    }

    // Cursor Blink will be always at the bottom of the screen
    if (blink)
        M5Cardputer.Display.fillRect(cursorX, cursorY, wp_font->glyphWidth, cursorHeight, foreground_color);
    else
        M5Cardputer.Display.fillRect(cursorX, cursorY, wp_font->glyphWidth, cursorHeight, background_color);
}

//
void WP_render_status()
{
    //
    JsonDocument &app = status();

    // LOAD COLORS
    uint16_t background_color = app["config"]["background_color"].as<uint16_t>();
    uint16_t foreground_color = app["config"]["foreground_color"].as<uint16_t>();

    int STATUSBAR_Y = M5Cardputer.Display.height() - 18;

    // file index number
    const int font_width = 12;
    int file_index = app["config"]["file_index"].as<int>();

    //
    M5Cardputer.Display.setTextSize(1);
    M5Cardputer.Display.setFont(&fonts::AsciiFont8x16);
    M5Cardputer.Display.setTextColor(background_color, foreground_color);
    M5Cardputer.Display.drawString(String(file_index), 4, STATUSBAR_Y);

    // WORD COUNT
    M5Cardputer.Display.setTextColor(foreground_color, background_color);

    int wordCount = Editor::getInstance().wordCountFile + Editor::getInstance().wordCountBuffer;
    String wordCountFormatted = formatNumber(wordCount);
    M5Cardputer.Display.drawString(wordCountFormatted, 30, STATUSBAR_Y);

    // SAVE STATUS
    if (Editor::getInstance().saved)
    {
        M5Cardputer.Display.fillCircle(screen_width - 15, STATUSBAR_Y + 8, 5, TFT_GREEN);
    }
    else
    {
        M5Cardputer.Display.fillCircle(screen_width - 15, STATUSBAR_Y + 8, 5, TFT_RED);
    }
    M5Cardputer.Display.drawCircle(screen_width - 15, STATUSBAR_Y + 8, 5, TFT_BLACK);

    // BATTERY
    static int displayedBattery = -1;     // the value shown on screen
    static int lastReadBattery = -1;      // last raw value read
    static uint32_t changeDetectedAt = 0; // when the change was first noticed

    int current = M5Cardputer.Power.getBatteryLevel();

    // First run
    if (displayedBattery < 0)
    {
        displayedBattery = current;
        lastReadBattery = current;
    }

    // If battery reading has not changed, reset timer & do nothing
    if (current == displayedBattery)
    {
        changeDetectedAt = 0;
        lastReadBattery = current;
    }
    // Battery reading changed (ex: 100 → 99)
    else
    {
        // If a change was just detected, start the timer
        if (changeDetectedAt == 0)
        {
            changeDetectedAt = millis();
            lastReadBattery = current;
        }

        // If reading fluctuates (ex: 100 → 99 → 100), cancel
        if (current != lastReadBattery)
        {
            changeDetectedAt = millis(); // restart timer with new reading
            lastReadBattery = current;
        }

        // If a second passed with stable new value → update UI
        if (millis() - changeDetectedAt >= 1000)
        {
            displayedBattery = current;
            changeDetectedAt = 0;
        }
    }

    // Draw smoothed / stabilized value
    M5Cardputer.Display.drawString(
        format("%d%%", displayedBattery),
        screen_width - 85,
        STATUSBAR_Y);
}

//
// Clear Screen
// Do it as less as possible so that there is the least amount of the flicker
//
void WP_render_clear()
{
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

    // clear background
    if (clear_background)
    {
        // when clearing background
        M5Cardputer.Display.fillRect(
            0,
            0,
            M5Cardputer.Display.width(),
            M5Cardputer.Display.height(),
            background_color);
    }
}

//
//
void WP_keyboard(int key, bool pressed, int index)
{
    // ignore non pritable keys
    if (key == 0)
        return;

    JsonDocument &app = status();
    _debug("WP_keyboard key: %d, pressed: %d\n", key, pressed);

    if (key == 27 || key == 6)
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
