#include "WordProcessor.h"
#include "../display_EPD.h"

//
#include "app/app.h"
#include "service/Editor/Editor.h"
#include "keyboard/keyboard.h"
#include "display/display.h"

// Generated Korean font - see script/generate_gfxfont_korean_font.py.
// GFXfont/4-bit-grayscale format (this backend's own, different from the
// u8g2 fonts used by RLCD/ILI9341/ST7735/CARDPUTER). Source is the same
// GalmuriMono11 (SIL OFL) used for Rev.8, rasterized larger (36px) for
// this panel's bigger 960x540 canvas; only included here, so its ~1.3MB
// of glyph data isn't duplicated into every other EPD screen's translation
// unit (unlike monospace.h/system.h, which are small enough not to matter).
#include "../fonts/galmuri_36_korean.h"

//
int last_sleep = millis();
//
#define MARGIN_X 20
#define MARGIN_Y 40

// Selectable word processor fonts. lineHeight is the hand-picked leading
// (larger than the font's own advance_y for readability on the big panel).
// Declared glyph widths are corrected at setup by reading the glyph table.
// The Korean font renders Hangul full-width, hence the double-width
// charColumns rule.
static EditorFont WP_FONTS[] = {
    {"monospace", &monospace, 20, 60, nullptr},
    {"korean36", &galmuri36Korean, 18, 40, editorfont_hangul_columns},
};
static const int WP_FONT_COUNT = sizeof(WP_FONTS) / sizeof(WP_FONTS[0]);

// active font, selected in WP_setup from config "font"
static EditorFont *wp_font = &WP_FONTS[0];

// status bar
const int status_height = 30;
const int statusY = EPD_HEIGHT - status_height - 5;

// measure the real glyph advance from the GFXfont glyph table
static int16_t WP_glyph_advance(const GFXfont *font, uint32_t codepoint)
{
    for (uint32_t i = 0; i < font->interval_count; i++)
    {
        const UnicodeInterval *interval = &font->intervals[i];
        if (codepoint >= interval->first && codepoint <= interval->last)
            return font->glyph[interval->offset + (codepoint - interval->first)].advance_x;
    }
    return 0;
}

// clear screen
bool clear_full = false;
bool clear_request = true;
bool cleared = true;

//
int startLine = -1;

// The composing Hangul character is tracked separately from the cursor.
// A composition can finish and immediately start again on the next jamo,
// so composer.isComposing() alone is not enough to detect a commit: a move
// of the composing character to another buffer position is also a commit.
static bool composing_prev = false;
static int composing_line_prev = -1;
static int composing_char_start_prev = -1;

//
void WP_setup()
{
    _debug("==========================================\n");
    _log("Word Processor GUI Setup\n");

    //
    JsonDocument &app = status();

    // pick the font from config ("font" is an index into WP_FONTS)
    int fontIndex = app["config"]["font"].as<int>();
    if (fontIndex < 0 || fontIndex >= WP_FONT_COUNT)
        fontIndex = 0;
    wp_font = &WP_FONTS[fontIndex];

    // measure the real glyph advance instead of trusting the declared value
    int16_t measured = WP_glyph_advance((const GFXfont *)wp_font->fontData, 'M');
    if (measured > 0)
        wp_font->glyphWidth = measured;

    // derive the layout from the font metrics:
    // text runs from the left margin to the panel edge, and from the top
    // margin down to the status bar
    int cols = (EPD_WIDTH - MARGIN_X) / wp_font->glyphWidth;
    int rows = (statusY - MARGIN_Y) / wp_font->lineHeight;

    //
    _log("WP layout font: %s %dx%d cols: %d rows: %d\n",
         wp_font->id, wp_font->glyphWidth, wp_font->lineHeight, cols, rows);

    // editor instantiate
    // Editor Init - setup screen size
    Editor::getInstance().init(cols, rows, wp_font);

    // display init
    display_initialize(MARGIN_X, MARGIN_Y, wp_font->lineHeight, wp_font->glyphWidth);

    // load file from the editor
    int file_index = app["config"]["file_index"].as<int>();

    //
    Editor::getInstance().loadFile(format("/%d.txt", file_index));

    //
    _log("Word Processor Initialized %d.txt\n", file_index);

    // clear background
    clear_full = true;

    // reset startLine
    startLine = -1;

    composing_prev = false;
    composing_line_prev = -1;
    composing_char_start_prev = -1;
}

//
void WP_render()
{
    // the editor swapped to a different window of the file (paging, or the
    // buffer filling up while typing) - force a full redraw
    if (Editor::getInstance().pageChanged)
    {
        Editor::getInstance().pageChanged = false;

        epd_poweron();
        epd_clear_quick(epd_full_screen(), 4, 50);
        epd_poweroff_all();

        startLine = -1;
        clear_request = true; // status bar should be refreshed too
    }

    // perform full screen clear routine
    if (clear_full)
    {
        //
        epd_poweron();
        epd_clear();
        epd_poweroff_all();

        //
        clear_full = false;
        clear_request = true;
    }

    // Clear Background
    if (clear_request)
    {
        //
        clear_request = false;

        // mark that the screen is cleared
        // so that following render functions will redraw
        cleared = true;

        //
        _debug("WP_render::full refresh requested\n");
    }

    //
    WP_check_saved();
    WP_check_sleep();

    // Bottom Status
    WP_render_status();

    // BLINK CURSOR
    WP_render_cursor();

    // RENDER TEXT
    WP_render_text();

    // clear background flag off
    cleared = false;

    // Editor House Keeping Tasks
    Editor::getInstance().loop();
}

// DRAW LINE OF TEXT
void WP_render_text_line(int i, int cursorY, uint8_t *framebuffer)
{
    char *line = Editor::getInstance().linePositions[i];
    int length = Editor::getInstance().lineLengths[i];

    //
    display_setline(i - startLine);

    // render
    if (line != NULL && length > 0)
    {
        // the buffer holds UTF-8 and writeln() decodes it directly
        char row[256];

        // Copy the line content to row, but not more than 255 bytes
        int copyLength = (length < 255) ? length : 255;
        strncpy(row, line, copyLength);

        // Null-terminate the string
        row[copyLength] = '\0';

        int cursorX = display_x();
        int cursorY = display_y();

        //
        if (framebuffer == NULL)
            epd_poweron();
        //
        writeln((GFXfont *)wp_font->fontData, row, &cursorX, &cursorY, framebuffer);

        //
        if (framebuffer == NULL)
            epd_poweroff_all();
    }
    else
    {
        _debug("WP_render_text_line::line null or empty line position %d\n", i);
    }
}

void WP_clear_row(int row)
{
    epd_poweron();

    // delete a line and redraw the line
    Rect_t area =
        display_rect(
            0,
            display_lineheight() * row,
            EPD_WIDTH,
            display_lineheight());

    epd_clear_quick(area, 4, 50);

    epd_poweroff_all();
}

// Clear and redraw one visible editor line. This is used when a Hangul
// composition is committed so any ghosting accumulated by the composing
// cell is cleaned up at the line level.
static void WP_refresh_text_line(int line)
{
    int row = line - startLine;
    if (row < 0 || row > Editor::getInstance().rows)
        return;

    WP_clear_row(row);
    display_setline(row);
    WP_render_text_line(line, display_y(), NULL);
}

// Refresh exactly the display cell occupied by the active composing Hangul
// character. Hangul and compatibility jamo use two editor columns, so the
// rectangle follows charColumns() rather than assuming a fixed byte/width.
static void WP_refresh_composing_cell(int line, int charStart)
{
    Editor &editor = Editor::getInstance();
    int row = line - startLine;
    if (row < 0 || row > editor.rows || line < 0 || line > editor.totalLine)
        return;

    char *linePtr = editor.linePositions[line];
    if (linePtr == NULL)
        return;

    int lineStart = (int)(linePtr - editor.buffer);
    int lineEnd = lineStart + editor.lineLengths[line];
    if (charStart < lineStart || charStart >= lineEnd)
        return;

    int prefixCols = 0;
    for (int pos = lineStart; pos < charStart;)
    {
        int charLen = 0;
        uint32_t codepoint = utf8_decode(editor.buffer + pos, &charLen);
        if (charLen <= 0)
            return;
        prefixCols += editor.charColumns(codepoint);
        pos += charLen;
    }

    int charLen = 0;
    uint32_t codepoint = utf8_decode(editor.buffer + charStart, &charLen);
    if (charLen <= 0 || charStart + charLen > lineEnd || charLen > 4)
        return;

    int x = MARGIN_X + display_fontwidth() * prefixCols;
    Rect_t area = display_rect(
        x,
        display_lineheight() * row,
        display_fontwidth() * editor.charColumns(codepoint),
        display_lineheight());

    char glyph[5];
    memcpy(glyph, editor.buffer + charStart, charLen);
    glyph[charLen] = '\0';

    display_setline(row);
    int cursorX = x;
    int cursorY = display_y();

    epd_poweron();
    epd_clear_quick(area, 4, 50);
    writeln((GFXfont *)wp_font->fontData, glyph, &cursorX, &cursorY, NULL);
    epd_poweroff_all();
}

//
bool editing = false;
void WP_render_text()
{
    //
    JsonDocument &app = status();

    // Cursor Information
    static int cursorPos_prev = 0;
    static int cursorLine_prev = 0;
    static int cursorLinePos_prev = 0;
    static int bufferSize_prev = 0;
    int cursorPos = Editor::getInstance().cursorPos;
    int cursorLine = Editor::getInstance().cursorLine;
    int cursorLinePos = Editor::getInstance().cursorLinePos;
    int bufferSize = Editor::getInstance().getBufferSize();

    Editor &editor = Editor::getInstance();
    bool composing = editor.composer.isComposing();
    int composingCharStart = composing ? editor.prevCharStart(cursorPos) : -1;
    bool compositionCommitted =
        composing_prev &&
        (!composing || cursorLine != composing_line_prev ||
         composingCharStart != composing_char_start_prev);

    //
    int totalLine = Editor::getInstance().totalLine;
    int rows = Editor::getInstance().rows;

    // when first turned on
    // start editing from last 2nd Line
    if (startLine == -1)
    {
        //
        startLine = max(cursorLine - rows + 1, 0);
    }

    //
    // when reaching the end of the screen reset the startLine
    if (cursorLine - startLine > rows)
    {
        // clear background
        epd_poweron();
        epd_clear_quick(epd_full_screen(), 4, 50);
        epd_poweroff_all();

        //
        cleared = true;
        clear_request = true; // status bar should be refreshed

        //
        startLine = max(cursorLine - 1, 0);
        _debug("WP_render_text::New Page cursorLine %d startLine %d rows %d totalLine %d\n",
               cursorLine,
               startLine,
               rows,
               totalLine);
    }

    // when cursor reaches the top and it's time to show the previous page
    if (cursorLine < startLine)
    {
        //
        _debug("WP_render_text::need to show previous page cursorLine %d prev %d startLine %d totalLine %d\n",
               cursorLine,
               cursorLine_prev,
               startLine,
               totalLine);

        // startLine should go one page before
        // keep the last line
        startLine = max(startLine - rows - 1, 0);

        _debug("WP_render_text::startLine updated %d \n",
               startLine);

        // clear background
        epd_poweron();
        epd_clear_quick(epd_full_screen(), 4, 50);
        epd_poweroff_all();

        //
        cleared = true;
    }

    //
    // Middle part of the text will be rendered
    // Only when refresh background is called
    //
    if (cleared)
    {
        // Draw from the first line
        display_setline(0);

        //
        for (int i = startLine; i <= totalLine; i++)
        {
            // stop when exceeding row
            if (i - startLine > rows)
            {
                _debug("WP_render_text::reached the end of the text startLine %d i %d row %d\n", startLine, i, rows);
                break;
            }

            if (i >= 0)
                WP_render_text_line(i, display_y(), display_EPD_framebuffer());

            // increase the line
            display_newline();
        }

        //
        // render frame to the display
        display_draw_buffer();
    }

    // The active composition ended, or feed() committed it and immediately
    // began the next Hangul cell. Clean up the completed line as one unit.
    else if (compositionCommitted)
    {
        editor.charReplaced = false;
        editor.backSpacePressed = false;

        WP_refresh_text_line(composing_line_prev);

        // Enter/wrapping can commit on the previous line and insert on a
        // new one in the same editor update. Make that new text visible too.
        if (cursorLine != composing_line_prev && bufferSize != bufferSize_prev)
            WP_refresh_text_line(cursorLine);
    }

    // While the same Hangul character is being assembled (ㄱ -> 가 -> 간),
    // restrict the EPD partial update to that character's display cell.
    else if (composing &&
             (editor.charReplaced || editor.backSpacePressed ||
              bufferSize != bufferSize_prev || cursorPos != cursorPos_prev))
    {
        editor.charReplaced = false;
        editor.backSpacePressed = false;
        WP_refresh_composing_cell(cursorLine, composingCharStart);
    }

    // handle backspace (whole-character deletion), and the rare composing
    // replace that crossed a line boundary - the row is cleared and
    // redrawn whole
    else if (Editor::getInstance().backSpacePressed || Editor::getInstance().charReplaced)
    {
        //
        Editor::getInstance().backSpacePressed = false;
        Editor::getInstance().charReplaced = false;

        // clear the currentLine and the previousLine
        _debug("WP_render_text::Handle Backspace\n");

        // delete a line and redraw the line
        WP_clear_row(max(cursorLine - startLine, 0));

        // if line is change then also clear the previous line
        if (cursorLine != cursorLine_prev)
        {
            WP_clear_row(max(cursorLine_prev - startLine, 0));
        }

        // and redraw the line
        WP_render_text_line(cursorLine, display_y(), NULL);
    }

    // Partial Refresh Logic
    else if (cleared == false && bufferSize != bufferSize_prev)
    {
        // new line
        // when new line is detected than redraw the previous line
        // in case when there is a word wrap then what was written at the previous line is moved to the current line
        if (cursorLine > cursorLine_prev)
        {
            // clear the currentLine and the previousLine
            _debug("WP_render_text::new line is detected. clear the previous line to complete word wrap.\n");

            WP_clear_row(max(cursorLine_prev - startLine, 0));

            // and redraw the line
            WP_render_text_line(cursorLine_prev, display_y(), NULL);

            // render the entire line
            cursorLinePos_prev = 0;
        }

        ////////////////////////////////////////////////
        // if currently writing in the middle for editing then erase the current line
        // and subsequent lines until the end of the screen
        if (cursorPos != bufferSize)
        {
            // mark editing flag
            editing = true;

            //
            int currentLine = max(0, cursorLine - startLine);

            _debug("WP_render_text::editing currentLine %d cursorLine %d cursorLineprev %d startLine %d\n", currentLine, cursorLine, cursorLine_prev, startLine);

            // Draw from the currentLine
            display_setline(currentLine);

            // delete a line and redraw the line
            WP_clear_row(currentLine);

            // and redraw the line
            WP_render_text_line(cursorLine, display_y(), NULL);
        }

        ////////////////////////////////////////////////

        //
        // Draw the new character entered
        else if (cursorPos != cursorPos_prev)
        {
            char *line = Editor::getInstance().linePositions[cursorLine];
            int line_length = Editor::getInstance().lineLengths[cursorLine];

            // pixel offset of the byte at cursorLinePos_prev: count the
            // display columns of the UTF-8 characters before it, since a
            // byte offset alone no longer maps 1:1 to a screen position
            int prefixCols = 0;
            for (int j = 0; j < cursorLinePos_prev && j < line_length;)
            {
                int charLen;
                uint32_t codepoint = utf8_decode(line + j, &charLen);
                prefixCols += Editor::getInstance().charColumns(codepoint);
                j += charLen;
            }

            // render entire line
            int cursorX = MARGIN_X + display_fontwidth() * prefixCols;
            display_setline(cursorLine - startLine);
            int cursorY = display_y();
            _debug("cursorY: %d cursorLine: %d\n", cursorY, cursorLine);

            _debug("cursorLine: %d line: %d line_length: %d cursorLinePos_prev: %d\n", cursorLine, line, line_length, cursorLinePos_prev);

            // new character has been typed
            if (cursorLinePos_prev < line_length)
            {
                // Copy the line tail from the previous cursor position,
                // but not more than 255 bytes - it is already UTF-8
                char row[256];
                int tailLength = line_length - cursorLinePos_prev;
                int copyLength = (tailLength < 255) ? tailLength : 255;
                strncpy(row, line + cursorLinePos_prev, copyLength);
                row[copyLength] = '\0';

                //
                epd_poweron();
                writeln((GFXfont *)wp_font->fontData, row, &cursorX, &cursorY, NULL);
                epd_poweroff_all();
            }
        }
    }

    // reset prev flags
    cursorPos_prev = cursorPos;
    cursorLinePos_prev = cursorLinePos;
    bufferSize_prev = bufferSize;

    composing_prev = composing;
    composing_line_prev = composing ? cursorLine : -1;
    composing_char_start_prev = composing ? composingCharStart : -1;

    // line changed
    if (cursorLine_prev != cursorLine)
    {
        if (editing == true)
        {
            // when line changes during the edit do full refresh

            // clear background
            epd_poweron();
            epd_clear_quick(epd_full_screen(), 4, 50);
            epd_poweroff_all();
            clear_request = true;

            editing = false;
            _debug("WP_render_text::editing line change. %d %d %d %d\n", cursorLine, cursorLine_prev, cursorPos, cursorPos_prev);
        }

        // reset prev flag
        cursorLine_prev = cursorLine;
    }
}

//
// Render Cursor
#define CURSOR_MARGIN 10
#define CURSOR_THICKNESS 5
#define CURSOR_DELAY 50

// fill a small buffer with solid black (nibble 0) and push only the
// cursor's tiny area through the proven image waveform, instead of
// a full-screen redraw or the unreliable raw pixel-push darken.
static void WP_draw_cursor(Rect_t area)
{
    int32_t cursor_buf_width = area.width / 2 + area.width % 2;
    uint8_t *cursor_buf = (uint8_t *)malloc(cursor_buf_width * area.height);
    if (cursor_buf)
    {
        memset(cursor_buf, 0x00, cursor_buf_width * area.height);

        epd_poweron();
        epd_draw_image(area, cursor_buf, BLACK_ON_WHITE);
        epd_poweroff_all();

        free(cursor_buf);
    }
}

// erase with a small padding margin so anti-aliased edges don't linger
// when the cursor is redrawn repeatedly at the same spot.
static void WP_erase_cursor(Rect_t area)
{
    Rect_t erase_area = display_rect(
        area.x - 10,
        area.y - 5,
        area.width + 20,
        area.height + 10);

    epd_poweron();
    epd_clear_quick(erase_area, 8, 50);
    epd_poweroff_all();
}

void WP_render_cursor()
{
    JsonDocument &app = status();

    // don't render at the round when screen is cleared
    if (cleared)
        return;

    // Cursor information
    static int renderedCursorX = -1;
    static bool cursorVisible = false;
    static int last = 0;

    static int cursorLineCols_prev = 0;
    static int cursorPos_prev = 0;
    int cursorLinePos = Editor::getInstance().cursorLinePos;
    int cursorLineCols = Editor::getInstance().cursorLineCols;
    int cursorLine = Editor::getInstance().cursorLine;
    int cursorPos = Editor::getInstance().cursorPos;

    static Rect_t area;

    //
    //

    // Calculate Cursor X position
    // reached the line where cursor is
    // cursor offset is measured in display columns so it stays correct
    // once double-width glyphs exist
    int cursorX = MARGIN_X;
    if (Editor::getInstance().buffer[max(cursorPos - 1, 0)] != '\n' && cursorLinePos != 0)
    {
        // where to display the cursor
        cursorX = MARGIN_X + cursorLineCols * display_fontwidth() + 5;

        //
        // _debug("WP_render_cursor::cursorX %d\n", cursorX);
    }

    // cursor moved: erase if currently visible and restart the idle timer
    if (cursorPos != cursorPos_prev)
    {
        if (cursorVisible)
        {
            WP_erase_cursor(area);
            cursorVisible = false;
        }

        // render the cursor
        renderedCursorX = -1;

        // reset the timer
        last = millis();

        //
        cursorPos_prev = cursorPos;
        cursorLineCols_prev = cursorLineCols;
    }

    // when there are no types for a short duration then
    // display the cursor
    if (renderedCursorX == -1 && last + CURSOR_DELAY < millis())
    {
        // Cursor will be always at the bottom of the screen
        area = display_rect(
            MARGIN_X + cursorLineCols * display_fontwidth(),
            MARGIN_Y + CURSOR_MARGIN + display_lineheight() * (max(cursorLine - startLine, 0)),
            display_fontwidth() * abs(cursorLineCols - cursorLineCols_prev + 1),
            CURSOR_THICKNESS);

        WP_draw_cursor(area);
        cursorVisible = true;

        //
        renderedCursorX = cursorX;
    }

    // NOTE: no idle blinking. Each draw_image call costs ~275ms on this
    // panel (15 waveform frames x full-height row scan, even for a tiny
    // area), so a blinking cursor kept the display task busy roughly a
    // third of the time and made everything else feel sluggish. The cursor
    // appears after a short pause and stays solid until it moves.
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
    if (millis() - last > 4000)
    {
        //
        last = millis();

        if (!Editor::getInstance().saved)
        {
            _log("saving file due in inactivity %d.txt\n", status()["config"]["file_index"].as<int>());
            Editor::getInstance().saveFile();
        }
    }
}

void WP_check_sleep()
{
    // 600 seconds - 10 minutes
    if (millis() - last_sleep > 10 * 60 * 1000)
    {
        // if no action for 10 minute go to sleep
        last_sleep = millis();

        //
        JsonDocument &app = status();
        app["screen"] = SLEEPSCREEN;
    }
}

// display file number
// display character total
// display keyboard layout - Rev.5 and Rev.7
// display save status
// FILE INDEX | BYTES | SAVED | LAYOUT
// 160 | 200 ---- 200 | 100
#define STATUS_REFRESH 300
void WP_render_status()
{
    //
    JsonDocument &app = status();

    // status start Y position
    int cursorY = statusY + status_height - 8;

    //
    static bool saved_prev = false;
    static size_t filesize_prev = 0;
    size_t filesize = Editor::getInstance().seekPos + Editor::getInstance().getBufferSize();

    // Full Redraw
    // Draw non-refreshing section
    if (cleared)
    {
        ////////////////////////////////////////
        // FILE INDEX 25 - 225
        int cursorX = 25;
        String file = format("FILE %d", app["config"]["file_index"].as<int>());
        writeln((GFXfont *)&systemFont, file.c_str(), &cursorX, &cursorY, display_EPD_framebuffer());
        ////////////////////////////////////////

        ////////////////////////////////////////
        // FILE SIZE
        cursorX = 200;
        String filesizeFormatted = formatNumber(filesize);
        writeln((GFXfont *)&systemFont, filesizeFormatted.c_str(), &cursorX, &cursorY, display_EPD_framebuffer());
        ////////////////////////////////////////

        ////////////////////////////////////////
        // KEYBOARD LAYOUT 860 - 960
        String layout = app["config"]["keyboard_layout"].as<String>();
        if (layout == "null" || layout.isEmpty())
            layout = "US"; // defaults to US layout
        cursorX = 860;
        writeln((GFXfont *)&systemFont, layout.c_str(), &cursorX, &cursorY, display_EPD_framebuffer());
        ////////////////////////////////////////
    }

    // FILE SIZE DRAWS WHEN STOPPED EDITING FOR A WHILE
    static int last = millis();
    static bool debouncing = false;
    if (filesize != filesize_prev || cleared)
    {
        // debounce for status_refresh amount
        last = millis();

        //
        filesize_prev = filesize;
        debouncing = true;
    }

    //
    if (debouncing == true && last + STATUS_REFRESH < millis())
    {
        //
        last = millis();
        debouncing = false;

        // FILE SIZE 200 - 400
        int cursorX = 200;

        // remove previous text
        Rect_t area = display_rect(
            cursorX - 10,
            statusY,
            210,
            status_height);

        epd_poweron();
        epd_clear_quick(area, 4, 50);

        // redraw the word count
        int wordCount = Editor::getInstance().wordCountFile + Editor::getInstance().wordCountBuffer; 
        String wordCountFormatted = formatNumber(wordCount);
        writeln((GFXfont *)&systemFont, wordCountFormatted.c_str(), &cursorX, &cursorY, NULL);
        epd_poweroff_all();
    }

    /////////////////////////////////////
    // DISPLAY SAVED STATE
    /////////////////////////////////////
    if (Editor::getInstance().saved != saved_prev || cleared)
    {
        // CHECK HERE FOR CRASH
        int cursorX = 550;

        // clear the status area
        Rect_t area = display_rect(
            cursorX - 10,
            statusY,
            210,
            status_height);

        //
        epd_poweron();
        epd_clear_quick(area, 4, 50);

        //
        String savedText = "NOT SAVED";
        if (Editor::getInstance().saved)
        {
            // file is saved
            savedText = "SAVED";
        }

        // display the text
        writeln((GFXfont *)&systemFont, savedText.c_str(), &cursorX, &cursorY, NULL);
        epd_poweroff_all();

        // previous values
        saved_prev = Editor::getInstance().saved;
    }
    /////////////////////////////////////
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
            _log("WP_keyboard::Menu Key Pressed. Saving file %d.txt\n", status()["config"]["file_index"].as<int>());
            Editor::getInstance().saveFile();

            //
            app["screen"] = MENUSCREEN;

            //
            _debug("WP_keyboard::Moving to Menu Screen\n");
        }
    }

    // REFRESH SCREEN F5
    else if (key == 5)
    {
        if (!pressed)
        {
            clear_full = true;
            _log("WP_keyboard::F5 Key Pressed. Saving file %d.txt\n", status()["config"]["file_index"].as<int>());
            Editor::getInstance().saveFile();
        }
    }

    // SLEEP BUTTON - PAUSE
    else if (key == 24)
    {
        if (!pressed)
        {
            // go to sleep mode
            app["screen"] = SLEEPSCREEN;
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
        // convert tab key to space
        if (key == '\t')
            key = ' ';

        // send the keys to the editor
        Editor::getInstance().keyboard(key, pressed);
    }
}

