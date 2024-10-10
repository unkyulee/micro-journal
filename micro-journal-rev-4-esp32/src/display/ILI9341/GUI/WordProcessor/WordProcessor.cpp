#include "WordProcessor.h"
#include "app/app.h"
#include "editor/editor.h"
#include "keyboard/keyboard.h"
#include "display/display.h"
#include "display/ILI9341/display_ILI9341.h"
#include "keyboard/hid/nimble/ble.h"

// 
int STATUSBAR_Y = 224;
int screen_width = 320;
int screen_height = 240;
bool clear_background = true;
unsigned int last_sleep = millis();

//
const int font_width = 12;
const int font_height = 18;
const int cursorY = 210;
const int cursorHeight = 2;
//
const int editY = cursorY - 4;

//
void WP_setup(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    // editor instantiate
    Editor::getInstance();
    STATUSBAR_Y = 224;

    // setup default color
    JsonDocument &app = app_status();
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
            Editor::getInstance().saveFile();
    }
}

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
        JsonDocument &app = app_status();
        app["screen"] = SLEEPSCREEN;
    }
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
    JsonDocument &app = app_status();

    // LOAD COLORS
    uint16_t background_color = app["config"]["background_color"].as<uint16_t>();
    uint16_t foreground_color = app["config"]["foreground_color"].as<uint16_t>();

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
}

//
void WP_render_line(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f, int line_num)
{
    char *line = Editor::getInstance().screenBuffer.line_position[line_num];
    int length = Editor::getInstance().screenBuffer.line_length[line_num];

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
void WP_render_text(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    JsonDocument &app = app_status();

    // LOAD COLORS
    uint16_t background_color = app["config"]["background_color"].as<uint16_t>();
    uint16_t foreground_color = app["config"]["foreground_color"].as<uint16_t>();

    // SET FONT
    pu8f->setFont(u8g2_font_profont22_tf);
    pu8f->setForegroundColor(foreground_color);
    pu8f->setFontMode(1);

    // Cursor Information
    static int cursorLine_prev = 0;
    static int cursorLinePos_prev = 0;
    int cursorLine = Editor::getInstance().fileBuffer.cursorLine;
    int cursorLinePos = Editor::getInstance().fileBuffer.cursorLinePos;
    int totalLine = Editor::getInstance().screenBuffer.total_line;

    //
    // Middle part of the text will be rendered
    // Only when refresh background is called
    //
    // initiate sprite
    TFT_eSprite sprite = display_ILI9341_sprite();
    if (clear_background)
    {
        sprite.createSprite(screen_width, screen_height);
        sprite.fillSprite(background_color);

        //
        pu8f->begin(sprite);
        pu8f->setFont(u8g2_font_profont22_mf);
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

// Status Bar
// - file index
// - current file size
// - keyboard layout
// - saved status
void WP_render_status(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    JsonDocument &app = app_status();

    // LOAD COLORS
    uint16_t background_color = app["config"]["background_color"].as<uint16_t>();
    uint16_t foreground_color = app["config"]["foreground_color"].as<uint16_t>();

    // RENDER STATUS BAR
    ptft->setTextColor(foreground_color, background_color);
    ptft->setTextSize(1);

    // FILE INDEX
    ptft->setCursor(0, STATUSBAR_Y, 2);
    ptft->setTextColor(background_color, foreground_color);
    ptft->printf(" %d ", app["config"]["file_index"].as<int>());

    // FILE SIZE
    ptft->setTextColor(foreground_color, background_color);
    ptft->setCursor(25, STATUSBAR_Y, 2);
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
    ptft->printf("%s characters", formattedNumber);

#ifdef ENV_USBHOST
    // KEYBOARD LAYOUT
    String layout = app["config"]["keyboard_layout"].as<String>();
    if (layout == "null" || layout.isEmpty())
        layout = "US"; // defaults to US layout
    // draw status bar
    ptft->setCursor(260, STATUSBAR_Y, 2);
    ptft->setTextColor(foreground_color, background_color);
    ptft->setTextSize(1);
    ptft->print(layout);

    // BLUETOOTH STATUS
    if (keyboard_ble_connected())
    {
        ptft->fillCircle(290, STATUSBAR_Y + 8, 5, TFT_BLUE);
    }
    else
    {
        // for now do not display disconnected status
        //ptft->fillCircle(290, STATUSBAR_Y + 8, 5, TFT_LIGHTGREY);
    }
    ptft->drawCircle(290, STATUSBAR_Y + 8, 5, TFT_BLACK);

#endif

    // SAVE STATUS
    if (Editor::getInstance().saved)
    {
        ptft->fillCircle(310, STATUSBAR_Y + 8, 5, TFT_GREEN);
    }
    else
    {
        ptft->fillCircle(310, STATUSBAR_Y + 8, 5, TFT_RED);
    }
    ptft->drawCircle(310, STATUSBAR_Y + 8, 5, TFT_BLACK);
}

//
// Render Cursor
void WP_render_cursor(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    JsonDocument &app = app_status();

    // retrieve color information
    uint16_t background_color = app["config"]["background_color"].as<uint16_t>();
    uint16_t foreground_color = app["config"]["foreground_color"].as<uint16_t>();

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
        ptft->fillRect(cursorLinePos_prev * font_width, cursorY, font_width, cursorHeight, background_color);

        //
        cursorLinePos_prev = cursorLinePos;
    }

    // Cursor Blink will be always at the bottom of the screen
    if (blink)
        ptft->fillRect(cursorX, cursorY, font_width, cursorHeight, foreground_color);
    else
        ptft->fillRect(cursorX, cursorY, font_width, cursorHeight, background_color);
}

//
void WP_keyboard(char key)
{
    // Check if menu key is pressed
    if (key == MENU)
    {
        // Save before transitioning to the menu
        Editor::getInstance().saveFile();

        //
        JsonDocument &app = app_status();
        app["screen"] = MENUSCREEN;
    }

    else
    {
        // send the keys to the editor
        Editor::getInstance().keyboard(key);
    }
}


// Create an array of String objects
static const String extended_ascii[128] = {
    "E", // 128 euro sign doesn't work on Micro Journal
    "",
    "", // 130
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "", // 140
    "",
    "Ž", // 142
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "", // 150
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "", // 160
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "", // 170
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "", // 180
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "", // 190
    "",
    "À",
    "Á",
    "Â",
    "Ã",
    "Ä",
    "Å",
    "Æ",
    "Ç",
    "È", // 200
    "É",
    "Ê",
    "Ë",
    "Ì",
    "Í",
    "Î",
    "Ï",
    "Ð",
    "Ñ",
    "Ò", // 210
    "Ó",
    "Ô",
    "Õ",
    "Ö",
    "×",
    "Ø",
    "Ù",
    "Ú",
    "Û",
    "Ü", // 220
    "Ý",
    "Þ",
    "ß",
    "à",
    "á",
    "â",
    "ã",
    "ä",
    "å",
    "æ", // 230
    "ç",
    "è",
    "é",
    "ê",
    "ë",
    "ì",
    "í",
    "î",
    "ï",
    "ð", // 240
    "ñ",
    "ò",
    "ó",
    "ô",
    "õ",
    "ö",
    "÷",
    "ø",
    "ù",
    "ú", // 250
    "û",
    "ü",
    "ý",
    "þ",
    "ÿ", // 255
};

String asciiToUnicode(uint8_t value)
{
  if (value < 128)
    return "";

  uint8_t code = value - 128;
  return extended_ascii[code];
}