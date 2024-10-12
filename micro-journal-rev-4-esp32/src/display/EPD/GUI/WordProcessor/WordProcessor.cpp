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
const int font_width = 30;
const int font_height = 50;
const int editY = 500;
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
    editor.screenBuffer.rows = 9;
    editor.screenBuffer.cols = 30;

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
    // show buffer
    if (draw)
    {
        epd_draw_grayscale_image(epd_full_screen(), display_EPD_framebuffer());
        memset(display_EPD_framebuffer(), 0xFF, EPD_WIDTH * EPD_HEIGHT / 2);

        app_log("Draw\n");
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
        clear_background = true;

    if (cursorPos_prev != cursorPos)
    {
        // if it is typing at the end don't flicker
        if (cursorPos != Editor::getInstance().fileBuffer.bufferSize)
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

    if (clear_background)
    {
        app_log("Clear Background\n");
        // clearing background
        epd_clear();
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

                // render
                if (line != NULL && length > 0)
                {
                    for (int j = 0; j < length; j++)
                    {
                        WP_render_char(line[j], MARGIN + font_width * j, cursorY);
                    }
                }
                cursorY += font_height;
            }
            
        }
    }

    //
    // Bottom line will the the edit area
    // Draw the last character
    if (cursorLinePos != cursorLinePos_prev && cursorLine == cursorLine_prev)
    {
        char *line = Editor::getInstance().screenBuffer.line_position[cursorLine];
        for (int i = cursorLinePos_prev; i < cursorLinePos; i++)
        {
            //
            WP_render_char(line[i], MARGIN + font_width * i, editY);
        }

        //
        cursorLinePos_prev = cursorLinePos;
    }

    if (cursorLine_prev != cursorLine)
    {
        cursorLine_prev = cursorLine;
    }
}

void WP_render_char(char c, int cursorX, int cursorY)
{
    // render
    char character[2];
    character[0] = c;
    character[1] = '\0';
    write_string((GFXfont *)&FiraSans, character, &cursorX, &cursorY, display_EPD_framebuffer());

    // show buffer
    draw = true;
}

//
// Render Cursor
void WP_render_cursor()
{
    JsonDocument &app = app_status();

    // Cursor information
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
        cursorX = cursorLinePos * font_width;
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
        Rect_t area = {
            .x = cursorLinePos_prev * font_width,
            .y = editY + MARGIN,
            .width = font_width,
            .height = cursorHeight};
        epd_push_pixels(area, 2, 1);

        //
        cursorLinePos_prev = cursorLinePos;
    }

    // Cursor Blink will be always at the bottom of the screen
    //
    Rect_t area = {
        .x = cursorX,
        .y = editY + MARGIN,
        .width = font_width,
        .height = cursorHeight};

    epd_push_pixels(area, 2, blink);
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
