#include "WordProcessor.h"
#include "../display_EPD.h"

//
#include "app/app.h"
#include "service/Editor/Editor.h"
#include "keyboard/keyboard.h"
#include "display/display.h"

//
int last_sleep = millis();
//
#define MARGIN_X 20
#define MARGIN_Y 40

//
const int cols = 47;
const int rows = 7;

// status bar
const int status_height = 30;
const int statusY = EPD_HEIGHT - status_height - 5;

// clear screen
bool clear_full = false;
bool clear_request = true;
bool cleared = true;

//
int startLine = -1;

//
void WP_setup()
{
    _debug("==========================================\n");
    _log("Word Processor GUI Setup\n");

    // editor instantiate
    // Editor Init - setup screen size
    Editor::getInstance().init(cols, rows);

    // display init
    GFXfont *font = display_EPD_font();
    display_initialize(MARGIN_X, MARGIN_Y, 60, 20);

    // load file from the editor
    JsonDocument &app = status();
    int file_index = app["config"]["file_index"].as<int>();

    //
    Editor::getInstance().loadFile(format("/%d.txt", file_index));

    //
    _log("Word Processor Initialized %d.txt\n", file_index);

    // clear background
    clear_full = true;

    // reset startLine
    startLine = -1;
}

//
void WP_render()
{
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
        char row[256];

        // Copy the line content to row, but not more than 255 characters
        int copyLength = (length < 255) ? length : 255;
        strncpy(row, line, copyLength);

        // Null-terminate the string
        row[copyLength] = '\0';
        char utf8[512];
        convert_extended_ascii_to_utf8(row, utf8, 512);

        int cursorX = display_x();
        int cursorY = display_y();

        //
        if (framebuffer == NULL)
            epd_poweron();
        //
        writeln(display_EPD_font(), utf8, &cursorX, &cursorY, framebuffer);

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

    // handle backspace
    else if (Editor::getInstance().backSpacePressed)
    {
        //
        Editor::getInstance().backSpacePressed = false;

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
            // render entire line
            int cursorX = MARGIN_X + display_fontwidth() * cursorLinePos_prev;
            display_setline(cursorLine - startLine);
            int cursorY = display_y();
            _debug("cursorY: %d cursorLine: %d\n", cursorY, cursorLine);

            char *line = Editor::getInstance().linePositions[cursorLine];
            int line_length = Editor::getInstance().lineLengths[cursorLine];

            _debug("cursorLine: %d line: %d line_length: %d cursorLinePos_prev: %d\n", cursorLine, line, line_length, cursorLinePos_prev);

            // new character has been typed
            if (cursorLinePos_prev < line_length)
            {
                // Copy the line content to row, but not more than 255 characters
                char row[256];
                int copyLength = (line_length < 255) ? line_length : 255;
                strncpy(row, line, copyLength);
                row[copyLength] = '\0';

                char utf8[512];
                convert_extended_ascii_to_utf8(row + cursorLinePos_prev, utf8, 512);

                //
                epd_poweron();
                writeln(display_EPD_font(), utf8, &cursorX, &cursorY, NULL);
                epd_poweroff();
            }

            
        }
    }

    // reset prev flags
    cursorPos_prev = cursorPos;
    cursorLinePos_prev = cursorLinePos;
    bufferSize_prev = bufferSize;

    // line changed
    if (cursorLine_prev != cursorLine)
    {
        if (editing == true)
        {
            // when line changes during the edit do full refresh

            // clear background
            epd_poweron();
            epd_clear_quick(epd_full_screen(), 4, 50);
            epd_poweroff();
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
void WP_render_cursor()
{
    JsonDocument &app = status();

    // don't render at the round when screen is cleared
    if (cleared)
        return;

    // Cursor information
    static int renderedCursorX = -1;
    static int last = 0;

    static int cursorLinePos_prev = 0;
    static int cursorPos_prev = 0;
    int cursorLinePos = Editor::getInstance().cursorLinePos;
    int cursorLine = Editor::getInstance().cursorLine;
    int cursorPos = Editor::getInstance().cursorPos;

    static Rect_t area;

    //
    //

    // Calculate Cursor X position
    // reached the line where cursor is
    // distance X is cursorPos - pos
    int cursorX = MARGIN_X;
    if (Editor::getInstance().buffer[max(cursorPos - 1, 0)] != '\n' && cursorLinePos != 0)
    {
        // where to display the cursor
        cursorX = MARGIN_X + cursorLinePos * display_fontwidth() + 5;

        //
        // _debug("WP_render_cursor::cursorX %d\n", cursorX);
    }

    // Delete previous cursor line
    if (cursorPos != cursorPos_prev)
    {
        if (renderedCursorX > 0)
        {
            //
            // _debug("Delete previous cursor line\n");

            //
            area = display_rect(
                area.x - 10,
                area.y - 5,
                area.width + 20,
                area.height + 10);

            epd_poweron();
            epd_clear_quick(area, 8, 50);
            epd_poweroff_all();

            // render the cursor
            renderedCursorX = -1;
        }

        // reset the timer
        last = millis();

        //
        cursorPos_prev = cursorPos;
        cursorLinePos_prev = cursorLinePos;
    }

    // when there are no types for a short duration then
    // display the cursor
    if (renderedCursorX == -1 && last + CURSOR_DELAY < millis())
    {
        // Cursor will be always at the bottom of the screen
        area = display_rect(
            MARGIN_X + cursorLinePos * display_fontwidth(),
            MARGIN_Y + CURSOR_MARGIN + display_lineheight() * (max(cursorLine - startLine, 0)),
            display_fontwidth() * abs(cursorLinePos - cursorLinePos_prev + 1),
            CURSOR_THICKNESS);

        //
        epd_fill_rect(area.x, area.y, area.width, area.height, 0, display_EPD_framebuffer());
        display_draw_buffer();

        //
        renderedCursorX = cursorX;
    }
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
        writeln((GFXfont *)&systemFont, filesizeFormatted.c_str(), &cursorX, &cursorY, NULL);
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
    if (filesize != filesize_prev)
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

        // redraw the new number
        String filesizeFormatted = formatNumber(filesize);
        writeln((GFXfont *)&systemFont, filesizeFormatted.c_str(), &cursorX, &cursorY, NULL);
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
void WP_keyboard(char key, bool pressed, int index)
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

    // REFRESH SCREEN F5
    else if (key == 5)
    {
        if (!pressed)
        {
            clear_full = true;
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

    else
    {
        // convert tab key to space
        if (key == '\t')
            key = ' ';

        // send the keys to the editor
        Editor::getInstance().keyboard(key, pressed);
    }
}

//
void convert_extended_ascii_to_utf8(const char *input, char *output, size_t output_size)
{
    size_t out_index = 0;

    for (size_t i = 0; input[i] != '\0' && out_index < output_size - 1; i++)
    {
        unsigned char ch = (unsigned char)input[i];

        if (ch < 128)
        {
            // ASCII character, copy directly.
            output[out_index++] = ch;
        }
        else
        {
            // Extended ASCII, encode in UTF-8 (2 bytes).
            if (out_index + 2 < output_size)
            {
                output[out_index++] = 0xC0 | (ch >> 6);   // First byte: 110xxxxx
                output[out_index++] = 0x80 | (ch & 0x3F); // Second byte: 10xxxxxx
            }
            else
            {
                // Not enough space for encoding.
                break;
            }
        }
    }

    // Null-terminate the output.
    output[out_index] = '\0';
}