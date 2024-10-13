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
int last_sleep = millis();
//
#define MARGIN 5
//
const int cols = 47;
const int rows = 8;
const int font_width = 20;
const int font_height = 49;
const int editY = font_height * (rows + 1);
const int statusY = EPD_HEIGHT - 4;

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
    editor.screenBuffer.rows = rows;
    editor.screenBuffer.cols = cols;

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

    //
    WP_check_saved();
    WP_check_sleep();

    // BLINK CURSOR
    WP_render_cursor();

    // BLINK CURSOR
    WP_render_status();

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

// Check if text is saved
void WP_check_saved()
{
    //
    static unsigned int last = millis();
    static int lastBufferSize = Editor::getInstance().fileBuffer.getBufferSize();
    int bufferSize = Editor::getInstance().fileBuffer.getBufferSize();

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
            clear_background = true;
        }
    }
}

void WP_check_sleep()
{
    /*
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
        JsonDocument &app = app_status();
        app["screen"] = SLEEPSCREEN;
    }
    */
}

// display file number
// display character total
// display keyboard layout - Rev.5 and Rev.7
// display save status
#define STATUS_REFRESH 30000
void WP_render_status()
{
    if (!clear_background)
        return;

    //
    JsonDocument &app = app_status();

    // redraw
    draw = true;

    // clear area
    Rect_t area = {
        .x = 0,
        .y = statusY,
        .width = EPD_WIDTH,
        .height = font_height,
    };
    area.width = EPD_WIDTH - area.x;
    epd_clear_quick(area, 8, 20);

    // FILE INDEX
    int cursorX = 4;
    int cursorY = statusY;
    String file = format("FILE %d", app["config"]["file_index"].as<int>());
    writeln((GFXfont *)&systemFont, file.c_str(), &cursorX, &cursorY, display_EPD_framebuffer());

    // FILE SIZE
    size_t num = Editor::getInstance().fileBuffer.seekPos + Editor::getInstance().fileBuffer.getBufferSize();
    String formattedNumber = "";
    int digitCount = 0;
    if (num < 0)
    {
        formattedNumber += "-";
        num = -num;
    }
    do
    {
        if (digitCount > 0 && digitCount % 3 == 0)
        {
            formattedNumber = "," + formattedNumber;
        }
        formattedNumber = String(num % 10) + formattedNumber;
        num /= 10;
        digitCount++;
    } while (num > 0);

    formattedNumber += " characters";
    cursorX = 140;
    writeln((GFXfont *)&systemFont, formattedNumber.c_str(), &cursorX, &cursorY, display_EPD_framebuffer());

    // KEYBOARD LAYOUT
    String layout = app["config"]["keyboard_layout"].as<String>();
    if (layout == "null" || layout.isEmpty())
        layout = "US"; // defaults to US layout

    cursorX = EPD_WIDTH - 120;
    writeln((GFXfont *)&systemFont, layout.c_str(), &cursorX, &cursorY, display_EPD_framebuffer());

    // BLUETOOTH STATUS
    if (keyboard_ble_connected())
    {
        epd_fill_circle(EPD_WIDTH - 70, statusY - 8, 8, 0, display_EPD_framebuffer());
    }

    // SAVE STATUS
    if (Editor::getInstance().saved)
    {
        epd_fill_circle(EPD_WIDTH - 30, statusY - 8, 8, 0, display_EPD_framebuffer());
    }
    else
    {
        epd_draw_circle(EPD_WIDTH - 30, statusY - 8, 8, 0, display_EPD_framebuffer());
    }
}

void WP_render_clear()
{
    static int erase_count = 0;

    // reset the flag
    if (clear_background)
        clear_background = false;

    // otherwise handle backspace
    JsonDocument &app = app_status();

    //
    static int cursorLine_prev = 0;
    static int cursorLinePos_prev = 0;
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
            .x = cursorLinePos * font_width,
            .y = editY - font_height,
            .width = EPD_WIDTH,
            .height = font_height,
        };
        area.width = EPD_WIDTH - area.x;
        epd_clear_quick(area, 8, 20);
    }

    if (cursorPos_prev != cursorPos)
    {
        // if it is typing at the end don't flicker
        if (cursorPos != Editor::getInstance().fileBuffer.bufferSize)
        {
            // if the edit line is empty then don't flicker
            //
            Rect_t area = {
                .x = cursorLinePos_prev * font_width,
                .y = editY - font_height,
                .width = EPD_WIDTH,
                .height = font_height,
            };
            area.width = EPD_WIDTH - area.x;
            epd_clear_quick(area, 4, 10);
        }

        //
        cursorPos_prev = cursorPos;
    }

    if (bufferSize != bufferSize_prev)
    {
        bufferSize_prev = bufferSize;
    }

    if (cursorLinePos_prev != cursorLinePos)
    {
        cursorLinePos_prev = cursorLinePos;
    }

    if (clear_background)
    {
        erase_count++;
        app_log("Clear Background\n");
        // clearing background
        if (erase_count > 3)
        {
            epd_clear();
            erase_count = 0;
        }

        else
        {
            epd_clear_quick(epd_full_screen(), 4, 10);
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
                    writeln(display_EPD_font(), row, &cursorX, &cursorY, display_EPD_framebuffer());
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
        writeln(display_EPD_font(), line, &cursorX, &cursorY, NULL);

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
    static int last = millis() + 2000;

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
                .height = 10};

            epd_clear_quick(area, 8, 50);

            // render the cursor
            renderedCursorX = -1;
        }

        // reset the timer
        last = millis();

        //
        cursorLinePos_prev = cursorLinePos;
    }

    // when there are no types for 5 seconds then
    // display the cursor
    if (renderedCursorX == -1 && last + 500 < millis())
    {
        // Cursor will be always at the bottom of the screen
        int cursorY = editY;
        writeln(display_EPD_font(), "_", &cursorX, &cursorY, NULL);

        //
        renderedCursorX = cursorX;
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
