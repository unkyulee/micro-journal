#include "WordProcessor.h"
#include "app/app.h"
#include "editor/editor.h"
#include "keyboard/keyboard.h"
#include "display/display.h"
#include "keyboard/ascii/ascii.h"

int STATUSBAR_Y = 224;
int screen_width = 320;
bool clear_background = true;
bool cleared_background = false;
unsigned int last_sleep = millis();

//
void WP_setup(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    // editor instantiate
    Editor::getInstance();
    STATUSBAR_Y = 224;

    // setup default color
    JsonDocument &app = app_status();
    if (!app["config"].containsKey("foreground_color"))
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
    Editor::getInstance().screenBuffer.loop();

    // CLEAR BACKGROUND
    WP_render_clear(ptft, pu8f);

    // RENDER STATUS BAR
    WP_render_status(ptft, pu8f);

    // BLINK CURSOR
    WP_render_cursor(ptft, pu8f);

    // RENDER TEXT
    WP_render_text(ptft, pu8f);

    // reset flags
    if (cleared_background)
    {
        cleared_background = false;
    }
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
    JsonDocument &app = app_status();

    // LOAD COLORS
    uint16_t background_color = app["config"]["background_color"].as<uint16_t>();
    uint16_t foreground_color = app["config"]["foreground_color"].as<uint16_t>();

    // check if the editor requests to clear screen
    if (Editor::getInstance().screenBuffer.clear)
    {
        // flag as clear completed nto the screen buffer
        Editor::getInstance().screenBuffer.clear = false;

        // set the flag to clear the background
        clear_background = true;
    }

    // clear background
    if (clear_background)
    {
        // reset flag
        clear_background = false;

        // this tell to the rest of the pipe line
        // that the screen is blank
        cleared_background = true;

        // clear screen with background color
        ptft->fillScreen(background_color);
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
    ptft->printf("%s bytes", formattedNumber);

#ifdef ENV_USBHOST
    // KEYBOARD LAYOUT
    String layout = app["config"]["keyboard_layout"].as<String>();
    if (layout == "null" || layout.isEmpty())
        layout = "US"; // defaults to US layout
    // draw status bar
    ptft->setCursor(280, STATUSBAR_Y, 2);
    ptft->setTextColor(foreground_color, background_color);
    ptft->setTextSize(1);
    ptft->print(layout);
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

void WP_render_text(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    JsonDocument &app = app_status();

    // LOAD COLORS
    uint16_t background_color = app["config"]["background_color"].as<uint16_t>();
    uint16_t foreground_color = app["config"]["foreground_color"].as<uint16_t>();

    // SET FONT
    pu8f->setFont(u8g2_font_profont22_mf);      // extended font
    pu8f->setForegroundColor(foreground_color); // apply color
    pu8f->setFontMode(1);                       // use u8g2 transparent mode (this is default)
    pu8f->setCursor(0, 24);                     // start writing at this position

    // TEXT WILL BE DISPLAYED
    // FROM TOP till the BOTTOM

    // start_line will maintain the TOP line
    // maximum of ROWs can be displayed on the screen
    int start_line = Editor::getInstance().screenBuffer.start_line;
    int total_line = Editor::getInstance().screenBuffer.total_line;

    // full render
    if (cleared_background)
    {
        for (int i = start_line; i <= total_line; i++)
        {
            // if the line goes out side the screen then stop printing
            if (i - start_line >= Editor::getInstance().screenBuffer.rows)
                break;

            //
            int length = Editor::getInstance().screenBuffer.line_length[i];
            char *line = Editor::getInstance().screenBuffer.line_position[i];

            // render
            for (int j = 0; j < length; j++)
            {
                // convert extended ascii into a streamlined string
                uint8_t value = *(line + j);

                // skip explicit new line
                if (value != '\n')
                    pu8f->print((char)value);
            }

            // new line
            pu8f->print("\n");
        }
    }
    else
    {
        // do partial render
        for (int i = start_line; i <= total_line; i++)
        {
            // if the line goes out side the screen then stop printing
            if (i - start_line >= Editor::getInstance().screenBuffer.rows)
                break;

            // render only if the range is around the cursor
            if (abs(i - Editor::getInstance().fileBuffer.cursorLine) <= 2)
            {
                //
                int length = Editor::getInstance().screenBuffer.line_length[i];
                char *line = Editor::getInstance().screenBuffer.line_position[i];

                // render
                bool stop = false;
                for (int j = 0; j < length; j++)
                {
                    // if the print reached the cursor then stop rendering
                    if (i >= Editor::getInstance().fileBuffer.cursorLine && j >= Editor::getInstance().fileBuffer.cursorLinePos)
                    {
                        stop = true;
                        break;
                    }

                    // convert extended ascii into a streamlined string
                    uint8_t value = *(line + j);

                    // skip explicit new line
                    if (value != '\n')
                        pu8f->print((char)value);
                }

                // stop at the cursor
                if (stop)
                    break;
            }

            // new line
            pu8f->print("\n");
        }
    }
}

void WP_render_cursor(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    JsonDocument &app = app_status();
    uint16_t background_color = app["config"]["background_color"].as<uint16_t>();
    uint16_t foreground_color = app["config"]["foreground_color"].as<uint16_t>();

    // current cursor information
    static int cursorPos_prev = 0;
    static int cursorLine_prev = 0;

    int cursorPos = Editor::getInstance().fileBuffer.cursorPos;
    int cursorLine = Editor::getInstance().fileBuffer.cursorLine;
    int cursorLinePos = Editor::getInstance().fileBuffer.cursorLinePos;
    int cursorLineLength = Editor::getInstance().screenBuffer.line_length[cursorLine];

    static int bufferSize_prev = 0;
    int bufferSize = Editor::getInstance().fileBuffer.getBufferSize();

    // calculate cursor positions
    // font size x 12 x 18
    // start y 24
    int cursorX;
    int cursorY;

    // reached the line where cursor is
    // distance X is cursorPos - pos
    cursorY = 6 + (cursorLine - Editor::getInstance().screenBuffer.start_line + 1) * 18;
    if (Editor::getInstance().fileBuffer.getBuffer()[cursorPos - 1] == '\n')
    {
        // new line characters doesn't count as an actual letter
        // but cursor should be rendered at the new line
        cursorX = 0;
    }
    else
    {
        cursorX = cursorLinePos * 12;
    }

    static bool blink = false;
    static unsigned int last = millis();
    if (millis() - last > 500)
    {
        last = millis();
        blink = !blink;
    }

    // draw the blink cursor
    if (blink)
        ptft->drawLine(cursorX, cursorY + 2, cursorX + 10, cursorY + 2, foreground_color);
    else
        ptft->drawLine(cursorX, cursorY + 2, cursorX + 10, cursorY + 2, background_color);

    // when new line or previous line changes
    if (cursorLine != cursorLine_prev)
    {
        // when going back one line above with backspace
        if (cursorLine < cursorLine_prev)
        {
            // delete the line below
            ptft->fillRect(0, cursorY, 320, 60, background_color);

            // delete the last character of the line
            ptft->fillRect(cursorX - 16, cursorY - 18, 320, 24, background_color);
        }

        // word wrap occurred
        // delete the previous line up
        if (cursorLineLength > 0)
        {
            // delete the last part of the previous line
            ptft->fillRect(320 - 12 * (cursorLineLength + 1), cursorY - 40, 320, 24, background_color);
        }

        // line changed remove the previous line trail
        // delete the previous trails
        ptft->drawLine(0, cursorY - 16, screen_width, cursorY - 16, background_color);

        //
        cursorLine_prev = cursorLine;

        //
        blink = true;
        last = millis();
    }

    // check cursor position changes
    if (cursorPos != cursorPos_prev)
    {
        // handle backspace
        if (cursorPos_prev > cursorPos)
        {
            // delete the character
            ptft->fillRect(cursorX, cursorY - 17, 320, 40, background_color);
        }

        // if the cursor is in the middle of the line
        // redraw all the trailing texts
        if (
            cursorPos != Editor::getInstance().fileBuffer.getBufferSize() &&
            bufferSize_prev != bufferSize)
        {
            ptft->fillRect(cursorX - 12, cursorY - 17, 320, 40, background_color);
            ptft->fillRect(0, cursorY, 320, 200 - cursorY, background_color);
        }

        // delete the previous trails
        ptft->drawLine(cursorX - (20 * (cursorPos - cursorPos_prev)), cursorY + 2, cursorX + 24, cursorY + 2, background_color);

        //
        cursorPos_prev = cursorPos;
        bufferSize_prev = bufferSize;

        //
        blink = true;
        last = millis();
    }
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
