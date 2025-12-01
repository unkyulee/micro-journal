#include "WordProcessor.h"
#include "app/app.h"

//
#include "service/Editor/Editor.h"
#include "keyboard/keyboard.h"
#include "display/display.h"

//
#include "display/CARDPUTER/display_CARDPUTER.h"

//
int screen_width = 240;
int screen_height = 135;

//
const int font_width = 14;
const int font_height = 20;

// Lines will be rendered at the bottom on the screen
// need to calculate the Y position considering the status bar height
const int editY = 100;
const int cursorY = editY + font_height - 2;
const int cursorHeight = 2;

// Some flags
bool clear_background = true;
unsigned int last_sleep = millis();

//
void WP_setup()
{
    // Editor Init - setup screen size
    Editor::getInstance().init(17, 5);

    // setup default color
    JsonDocument &app = status();

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
    M5Cardputer.Display.setFont(&fonts::FreeMonoBold12pt7b);

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
            y += font_height;
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

    if (length > 0)
    {
        // Create a temporary null-terminated buffer
        char temp[length + 1];
        memcpy(temp, line, length);
        temp[length] = '\0';

        //
        M5Cardputer.Display.drawString(temp, 0, y);
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
    static int cursorLinePos_prev = 0;
    int cursorLinePos = Editor::getInstance().cursorLinePos;
    int cursorLine = Editor::getInstance().cursorLine;
    int cursorPos = Editor::getInstance().cursorPos;

    // Calculate Cursor X position
    // reached the line where cursor is
    // distance X is cursorPos - pos
    int cursorX = 0;
    if (Editor::getInstance().buffer[cursorPos - 1] != '\n' && cursorLinePos != 0)
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
        M5Cardputer.Display.fillRect(cursorLinePos_prev * font_width, cursorY, font_width, cursorHeight, background_color);

        //
        cursorLinePos_prev = cursorLinePos;
    }

    // Cursor Blink will be always at the bottom of the screen
    if (blink)
        M5Cardputer.Display.fillRect(cursorX, cursorY, font_width, cursorHeight, foreground_color);
    else
        M5Cardputer.Display.fillRect(cursorX, cursorY, font_width, cursorHeight, background_color);
}

//
void WP_render_status()
{
    //
    JsonDocument &app = status();

    // LOAD COLORS
    uint16_t background_color = app["config"]["background_color"].as<uint16_t>();
    uint16_t foreground_color = app["config"]["foreground_color"].as<uint16_t>();

    int STATUSBAR_Y = cursorY + cursorHeight + 2;

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
    int battery = M5Cardputer.Power.getBatteryLevel();
    M5Cardputer.Display.drawString(format("%d%%", battery), screen_width - 85, STATUSBAR_Y);
}

//
// Clear Screen
// Do it as less as possible so that there is the least amount of the flicker
//
void WP_render_clear()
{
    //
    JsonDocument &app = status();

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


