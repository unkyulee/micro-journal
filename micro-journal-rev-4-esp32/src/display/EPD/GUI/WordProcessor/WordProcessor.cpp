#include "WordProcessor.h"
#include "../../display_EPD.h"

//
#include "app/app.h"
#include "editor/editor.h"
#include "keyboard/keyboard.h"
#include "display/display.h"
#include "keyboard/hid/nimble/ble.h"

//
bool clear_background = true;
bool draw = false;

//
#define MARGIN 10
//
const int font_width = 25;
const int font_height = 50;
const int editY = 450;
const int cursorHeight = 5;

//
void WP_setup()
{
    app_log("Word Processor GUI Setup\n");

    // Turn on the display
    epd_poweron();

    // Clear Screen
    epd_clear();

    // Turn off the display
    epd_poweroff();

    // editor instantiate
    Editor &editor = Editor::getInstance();
    editor.screenBuffer.rows = 8;
    editor.screenBuffer.cols = 50;

    // load file from the editor
    JsonDocument &app = app_status();
    int file_index = app["config"]["file_index"].as<int>();

    //
    Editor::getInstance().loadFile(format("/%d.txt", file_index));

    //
    app_log("Word Processor Initialized %d.txt\n", file_index);

    //
    clear_background = true;
}

//
void WP_render()
{
    // reset draw flag
    draw = false;

    // Turn on the display
    epd_poweron();

    // CLEAR BACKGROUND
    WP_render_clear();

    // BLINK CURSOR
    WP_render_cursor();

    // RENDER TEXT
    WP_render_text();

    //
    if (draw)
    {
        // render frame to the display
        epd_draw_grayscale_image(epd_full_screen(), display_EPD_framebuffer());
        memset(display_EPD_framebuffer(), 0xFF, EPD_WIDTH * EPD_HEIGHT / 2);
    }

    // Turn off the display
    epd_poweroff();
}

void WP_render_clear()
{
    // reset the flag
    if (clear_background)
        clear_background = false;

    // otherwise handle backspace
    JsonDocument &app = app_status();

    //
    static int cursorLine_prev = 0;
    static int cursorPos_prev = 0;
    int cursorLine = Editor::getInstance().fileBuffer.cursorLine;
    int cursorPos = Editor::getInstance().fileBuffer.cursorPos;
    int cursorLinePos = Editor::getInstance().fileBuffer.cursorLinePos;
    int cursorLineLength = Editor::getInstance().screenBuffer.line_length[cursorLine];

    //
    static int bufferSize_prev = 0;
    int bufferSize = Editor::getInstance().fileBuffer.bufferSize;

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
    {
        // backspace
        Rect_t area = {
            .x = 0,
            .y = editY - font_height,
            .width = EPD_WIDTH,
            .height = EPD_HEIGHT,
        };
        epd_clear_quick(area, 4, 10);
    }

    if (cursorPos_prev != cursorPos)
    {
        // if it is typing at the end don't flicker
        if (cursorPos != Editor::getInstance().fileBuffer.bufferSize)
        {
            // if the edit line is empty then don't flicker
            //
            Rect_t area = {
                .x = 0,
                .y = editY - font_height,
                .width = EPD_WIDTH,
                .height = EPD_HEIGHT,
            };
            epd_clear_quick(area, 4, 10);
        }

        //
        cursorPos_prev = cursorPos;
    }

    if (bufferSize != bufferSize_prev)
    {
        bufferSize_prev = bufferSize;
    }

    if (clear_background)
    {
        static int erase_count = 0;
        erase_count++;
        app_log("Clear Background\n");
        // clearing background
        if (erase_count > 10)
        {
            epd_clear();
            erase_count = 0;
        }

        else
        {
            epd_clear_quick(epd_full_screen(), 8, 10);
        }
    }
}

//
void WP_render_text()
{
    //
    JsonDocument &app = app_status();

    // Cursor Information
    static int cursorLinePos_prev = 0;
    static int cursorLine_prev = 0;
    int cursorLine = Editor::getInstance().fileBuffer.cursorLine;
    int cursorLinePos = Editor::getInstance().fileBuffer.cursorLinePos;

    //
    // Middle part of the text will be rendered
    // Only when refresh background is called
    //
    if (clear_background)
    {
        // start line
        int rows = Editor::getInstance().screenBuffer.rows;
        int cursorY = font_height;
        for (int i = cursorLine - rows; i <= cursorLine; i++)
        {
            if (i >= 0)
            {
                char *line = Editor::getInstance().screenBuffer.line_position[i];
                int length = Editor::getInstance().screenBuffer.line_length[i];
                int cursorX = MARGIN;
                // render
                if (line != NULL && length > 0)
                {
                    char row[256];

                    // Copy the line content to row, but not more than 255 characters
                    int copyLength = (length < 255) ? length : 255;
                    strncpy(row, line, copyLength);

                    // Null-terminate the string
                    row[copyLength] = '\0';
                    writeln((GFXfont *)&FiraSans, row, &cursorX, &cursorY, display_EPD_framebuffer());
                }
            }

            cursorY += font_height;
        }

        // buffer needs to be rendered
        draw = true;
    }

    //
    // Bottom line will the the edit area
    // Draw the last character
    if (cursorLinePos != cursorLinePos_prev)
    {
        // render entire line
        int cursorX = MARGIN;
        int cursorY = editY;
        char *line = Editor::getInstance().screenBuffer.line_position[cursorLine];
        writeln((GFXfont *)&FiraSans, line, &cursorX, &cursorY, NULL);

        //
        cursorLinePos_prev = cursorLinePos;
    }

    if (cursorLine_prev != cursorLine)
    {
        cursorLine_prev = cursorLine;
    }
}

//
// Render Cursor
void WP_render_cursor()
{
    JsonDocument &app = app_status();

    // Cursor information
    static int renderedCursorX = -1;
    static bool last = millis() + 2000;

    static int cursorLinePos_prev = 0;
    int cursorLinePos = Editor::getInstance().fileBuffer.cursorLinePos;
    int cursorLine = Editor::getInstance().fileBuffer.cursorLine;
    int cursorPos = Editor::getInstance().fileBuffer.cursorPos;

    // Calculate Cursor X position
    // reached the line where cursor is
    // distance X is cursorPos - pos
    int cursorX = 0;
    if (Editor::getInstance().fileBuffer.buffer[cursorPos - 1] != '\n' && cursorLinePos != 0)
    {
        // font width 12
        cursorX = MARGIN + cursorLinePos * font_width;
    }

    // Delete previous cursor line
    if (cursorLinePos != cursorLinePos_prev)
    {
        if (renderedCursorX > 0)
        {
            //
            Rect_t area = {
                .x = 0,
                .y = editY,
                .width = EPD_WIDTH,
                .height = 20};

            epd_clear_quick(area, 8, 50);
            app_log("clear cursor: %d\n", renderedCursorX);

            // render the cursor
            renderedCursorX = -1;
        }

        // reset the timer
        last = millis();

        //
        cursorLinePos_prev = cursorLinePos;
    }

    //
    if (renderedCursorX == -1 && last < millis() + 1000)
    {
        // Cursor will be always at the bottom of the screen
        int cursorY = editY;
        cursorX += 5;
        writeln((GFXfont *)&FiraSans, "_", &cursorX, &cursorY, NULL);

        //
        renderedCursorX = cursorX;
        app_log("Cursor: %d\n", renderedCursorX);
    }
}

//
void WP_keyboard(char key)
{
    JsonDocument &app = app_status();

    // Check if menu key is pressed
    if (key == MENU)
    {
        // Save before transitioning to the menu
        Editor::getInstance().saveFile();

        //
        app["screen"] = MENUSCREEN;
    }

    else
    {
        // send the keys to the editor
        Editor::getInstance().keyboard(key);
    }
}
