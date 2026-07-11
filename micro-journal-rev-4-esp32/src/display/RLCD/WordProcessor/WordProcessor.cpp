#include "WordProcessor.h"
#include "app/app.h"
#include "keyboard/keyboard.h"
#include "display/display.h"
#include "display/RLCD/display_RLCD.h"

//
#include "service/Editor/Editor.h"

//
int STATUSBAR_Y = 295;
int screen_width = 400;
int screen_height = 300;
bool clear_background = true;
unsigned int last_sleep = millis();

// Whether the last WP_render() actually changed anything visible - the
// caller uses this to decide whether the (expensive, full 30KB SPI) panel
// refresh is worth doing this tick.
static bool needsDisplay = true;
bool WP_contentChanged();

// Generated Korean font - see fonts/ and script/generate_u8g2_korean_font.py.
// Source: GalmuriMono11 (SIL OFL), a hand-designed pixel font that is
// natively fixed-width with Hangul exactly 2x the Latin advance - unlike
// scalable coding fonts (D2Coding, NanumGothicCoding), its stems are drawn
// on an integer pixel grid, so every stroke rasterizes to a uniform
// thickness instead of alternating 2px/3px depending on hinting rounding.
extern const uint8_t u8g2_font_galmuri_24_t_korean[];

// Selectable word processor fonts. Declared metrics are corrected at setup
// by measuring the actual font, so nothing here has to be exact by hand.
// The Korean fonts render Hangul full-width, hence the double-width
// charColumns rule. "korean32" is GNU Unifont pixel-doubled (scale 2) -
// kept as a fallback alternative.
static EditorFont WP_FONTS[] = {
    {"profont22", u8g2_font_profont22_mf, 12, 22, nullptr, 1},
    {"profont29", u8g2_font_profont29_mf, 16, 29, nullptr, 1},
    {"korean24", u8g2_font_galmuri_24_t_korean, 12, 26, editorfont_hangul_columns, 1},
    {"korean32", u8g2_font_unifont_t_korean2, 16, 32, editorfont_hangul_columns, 2},
};
static const int WP_FONT_COUNT = sizeof(WP_FONTS) / sizeof(WP_FONTS[0]);

// active font, selected in WP_setup from config "font"
static EditorFont *wp_font = &WP_FONTS[0];

// layout derived from the active font metrics in WP_setup
static int editY = 264;   // baseline of the edit line
static int cursorY = 270; // top of the cursor band under the edit line
const int cursorHeight = 2;
const int cursorGap = 6; // space between edit line baseline and cursor band
const int marginX = 5;

//
void WP_setup(ST7305_4p2_BW_DisplayDriver *display, U8G2_FOR_ST73XX *u8)
{
    // setup default color
    JsonDocument &app = status();

    // pick the font from config ("font" is an index into WP_FONTS)
    int fontIndex = app["config"]["font"].as<int>();
    if (fontIndex < 0 || fontIndex >= WP_FONT_COUNT)
        fontIndex = 0;
    wp_font = &WP_FONTS[fontIndex];

    // measure the real glyph advance instead of trusting the declared value
    // (setScale after setFont - changing the font resets the scale, and the
    // measurement comes back already scaled).
    // Measured as a difference: getUTF8Width() substitutes the LAST glyph's
    // tight bitmap width for its advance, so a single "M" under-reports on
    // fonts with trimmed glyph boxes - the difference cancels that and
    // leaves exactly one true advance.
    u8->setFont((const uint8_t *)wp_font->fontData);
    u8->setScale(wp_font->scale);
    int16_t measured = u8->getUTF8Width("MM") - u8->getUTF8Width("M");
    if (measured > 0)
        wp_font->glyphWidth = measured;

    // derive the layout from the font metrics:
    // the text area ends at the status bar divider (STATUSBAR_Y - 16),
    // minus room for the cursor band that sits under the edit line
    int rows = (STATUSBAR_Y - 16 - cursorGap - cursorHeight) / wp_font->lineHeight;
    int cols = (screen_width - 2 * marginX) / wp_font->glyphWidth;
    editY = rows * wp_font->lineHeight;
    cursorY = editY + cursorGap;

    //
    _log("WP layout font: %s %dx%d cols: %d rows: %d editY: %d\n",
         wp_font->id, wp_font->glyphWidth, wp_font->lineHeight, cols, rows, editY);

    // editor instantiate
    Editor::getInstance().init(cols, rows, wp_font);

    // load file from the editor
    int file_index = app["config"]["file_index"].as<int>();
    Editor::getInstance().loadFile(format("/%d.txt", file_index));

    // start from clear background
    clear_background = true;
    display->clearDisplay();
    display->display();

    // sleep timer reset
    last_sleep = millis();
}

//
void WP_render(ST7305_4p2_BW_DisplayDriver *display, U8G2_FOR_ST73XX *u8)
{
    // the editor swapped to a different window of the file (paging, or the
    // buffer filling up while typing) - force a full redraw
    if (Editor::getInstance().pageChanged)
    {
        Editor::getInstance().pageChanged = false;
        display->clearDisplay();
        clear_background = true;
    }

    // timers
    WP_check_saved();

    // CLEAR BACKGROUND
    WP_render_clear(display, u8);

    // RENDER TEXT
    WP_render_text(display, u8);

    // RENDER STATUS BAR
    WP_render_status(display, u8);

    // BLINK CURSOR
    WP_render_cursor(display, u8);

    // decide whether the panel actually needs to be pushed over SPI this tick
    needsDisplay = WP_contentChanged();

    //
    if (clear_background)
        clear_background = false;

    // Editor House Keeping Task
    Editor::getInstance().loop();

}

bool WP_needsDisplay()
{
    return needsDisplay;
}

// Compares the values that actually drive what's on screen against the
// last time this was checked. The draw calls above always run (they just
// write into the in-RAM frame buffer, which is cheap) - this only decides
// whether the buffer is worth pushing over SPI to the physical panel.
bool WP_contentChanged()
{
    static int cursorPos_prev = -1;
    static int cursorLinePos_prev = -1;
    static int bufferSize_prev = -1;
    static bool saved_prev = true;
    static int wordCount_prev = -1;
    static int file_index_prev = -1;
    static int blinkPhase_prev = -1;

    JsonDocument &app = status();

    int cursorPos = Editor::getInstance().cursorPos;
    int cursorLinePos = Editor::getInstance().cursorLinePos;
    int bufferSize = Editor::getInstance().getBufferSize();
    bool saved = Editor::getInstance().saved;
    int wordCount = Editor::getInstance().wordCountFile + Editor::getInstance().wordCountBuffer;
    int file_index = app["config"]["file_index"].as<int>();
    // flips every 500ms, matching WP_render_cursor()'s blink rate
    int blinkPhase = (millis() / 500) % 2;

    bool changed = clear_background ||
                   cursorPos != cursorPos_prev ||
                   cursorLinePos != cursorLinePos_prev ||
                   bufferSize != bufferSize_prev ||
                   saved != saved_prev ||
                   wordCount != wordCount_prev ||
                   file_index != file_index_prev ||
                   blinkPhase != blinkPhase_prev;

    cursorPos_prev = cursorPos;
    cursorLinePos_prev = cursorLinePos;
    bufferSize_prev = bufferSize;
    saved_prev = saved;
    wordCount_prev = wordCount;
    file_index_prev = file_index;
    blinkPhase_prev = blinkPhase;

    return changed;
}

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
    // when idle for 4 seconds then auto save
    if (millis() - last > 2000)
    {
        //
        last = millis();

        if (!Editor::getInstance().saved)
            Editor::getInstance().saveFile();
    }
}

//
// Clear Screen
// Do it as less as possible so that there is the least amount of the flicker
//
void WP_render_clear(ST7305_4p2_BW_DisplayDriver *display, U8G2_FOR_ST73XX *u8)
{
    // clear background
    if (clear_background)
    {
        // screen buffer will be pushed after
        return;
    }

    // the composing Hangul character was replaced in place - the new glyph
    // doesn't necessarily cover the old one's pixels, so repaint fully
    if (Editor::getInstance().charReplaced)
    {
        Editor::getInstance().charReplaced = false;
        clear_background = true;
    }

    //
    JsonDocument &app = status();

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
        if (cursorPos != Editor::getInstance().getBufferSize())
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

    //
    if (clear_background)
    {
        // clear the screen buffer so the next render will get the clean
        display->clearDisplay();
    }
}

//
void WP_render_line(ST7305_4p2_BW_DisplayDriver *display, U8G2_FOR_ST73XX *u8, int line_num)
{
    char *line = Editor::getInstance().linePositions[line_num];
    int length = Editor::getInstance().lineLengths[line_num];

    // render - the buffer holds UTF-8 and the u8g2 print pipeline decodes
    // multi-byte characters itself, so bytes stream straight through
    for (int i = 0; i < length; i++)
    {
        char value = *(line + i);
        if (value == '\n')
            continue;

        u8->print(value);
    }
}

//
void WP_render_text(ST7305_4p2_BW_DisplayDriver *display, U8G2_FOR_ST73XX *u8)
{
    JsonDocument &app = status();

    // SET FONT
    u8->setFont((const uint8_t *)wp_font->fontData);
    u8->setScale(wp_font->scale);

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
        //
        u8->setFont((const uint8_t *)wp_font->fontData);
        u8->setScale(wp_font->scale);

        // start line
        int rows = Editor::getInstance().rows;

        // I think
        int startLine = cursorLine - rows;
        for (int i = cursorLine - rows; i < cursorLine; i++)
        {
            //
            u8->setCursor(marginX, wp_font->lineHeight * (i - startLine));

            if (i >= 0) 
                WP_render_line(display, u8, i);
        }
    }

    //
    // Bottom line will the the edit area
    //
    u8->setCursor(marginX, editY);
    WP_render_line(display, u8, cursorLine);
}

// Status Bar
// - file index
// - current file size
// - keyboard layout
// - saved status
void WP_render_status(ST7305_4p2_BW_DisplayDriver *display, U8G2_FOR_ST73XX *u8)
{
    JsonDocument &app = status();

    // STATUS BAR
    display->drawLine(0, STATUSBAR_Y - 16, screen_width, STATUSBAR_Y - 16, 1);

    // FILE INDEX
    u8->setFont(u8g2_font_profont17_tf);
    u8->setScale(1);
    u8->setCursor(5, STATUSBAR_Y);
    u8->printf("FILE %d ", app["config"]["file_index"].as<int>());

    // WORD COUNT
    u8->setCursor(100, STATUSBAR_Y);

    int wordCount = Editor::getInstance().wordCountFile + Editor::getInstance().wordCountBuffer;
    String wordCountFormatted = formatNumber(wordCount);
    u8->printf("%s WORDS", wordCountFormatted);

    // LOCALE
    String locale = app["config"]["keyboard_layout"].as<String>();
    if (locale.length() > 0 && locale != "US" && locale != "null")
    {
        u8->setCursor(280, STATUSBAR_Y);
        u8->printf(locale.c_str());
    }

    // SAVE STATUS    
    if (Editor::getInstance().saved)
    {
        // when saved write SAVED
        u8->setCursor(350, STATUSBAR_Y);
        u8->printf("SAVED");
    }
    else
    {
        // don't show "SAVED"
        display->drawFilledRectangle(200, STATUSBAR_Y - 12, screen_width, screen_height, 0);
    }
}

//
// Render Cursor
void WP_render_cursor(ST7305_4p2_BW_DisplayDriver *display, U8G2_FOR_ST73XX *u8)
{
    JsonDocument &app = status();

    // Cursor information
    static int cursorLinePos_prev = 0;
    int cursorLinePos = Editor::getInstance().cursorLinePos;
    int cursorLineCols = Editor::getInstance().cursorLineCols;
    int cursorLine = Editor::getInstance().cursorLine;
    int cursorPos = Editor::getInstance().cursorPos;

    // Calculate Cursor X position
    // reached the line where cursor is
    // cursor offset is measured in display columns so it stays correct
    // once double-width glyphs exist
    int cursorX = marginX;
    if (Editor::getInstance().buffer[cursorPos - 1] != '\n' && cursorLinePos != 0)
    {
        // give a bit of margin
        cursorX = cursorLineCols * wp_font->glyphWidth + marginX;
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
    if (cursorLinePos != cursorLinePos_prev)
    {
        //
        display->drawFilledRectangle(
            0,
            cursorY,
            screen_width,
            cursorY + cursorHeight,
            0);

        //
        cursorLinePos_prev = cursorLinePos;
    }

    // Cursor Blink will be always at the bottom of the screen
    if (blink)
        display->drawFilledRectangle(cursorX, cursorY, cursorX + wp_font->glyphWidth, cursorY + cursorHeight, 1);
    else
        display->drawFilledRectangle(cursorX, cursorY, cursorX + wp_font->glyphWidth, cursorY + cursorHeight, 0);
}

//
void WP_keyboard(int key, bool pressed, int index)
{
    // Every key stroke resets sleep timer
    last_sleep = millis();

    //
    JsonDocument &app = status();

    // Check if menu key is pressed
    if (key == MENU || key == 27) // or ESC key
    {
        if (!pressed)
        {
            // Save before transitioning to the menu
            Editor::getInstance().saveFile();

            //
            app["screen"] = MENUSCREEN;

            //
            _debug("WP_keyboard::Moving to Menu Screen\n");
        }
    }

    // Check if File Change request is pressed
    else if (key >= 1000 && key <= 1010)
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
