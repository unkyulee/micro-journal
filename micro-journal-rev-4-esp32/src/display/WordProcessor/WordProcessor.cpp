#include "WordProcessor.h"
#include "app/app.h"
#include "editor/editor.h"
#include "keyboard/keyboard.h"
#include "display/display.h"
#include "keyboard/ascii/ascii.h"

// font size x 12 x 18
//
int STATUSBAR_Y = 224;
int screen_width = 320;
bool clear_background = true;
bool cleared_background = false;
unsigned int last_sleep = millis();

//
void WP_setup(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    // editor instantiate
    Editor::getInstance(10, 26);
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

    // CLEAR BACKGROUND
    WP_render_clear(ptft, pu8f);

    // RENDER STATUS BAR
    WP_render_status(ptft, pu8f);

    // RENDER TEXT
    WP_render_text(ptft, pu8f);

    // BLINK CURSOR
    WP_render_blink(ptft, pu8f);

    //
    if (cleared_background)
        cleared_background = false;
}

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
    ptft->printf("%s bytes", Editor::getInstance().getFileSize());

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
}

void WP_render_text(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    JsonDocument &app = app_status();

    // LOAD COLORS
    uint16_t background_color = app["config"]["background_color"].as<uint16_t>();
    uint16_t foreground_color = app["config"]["foreground_color"].as<uint16_t>();

    // Render the user text
    pu8f->setFont(u8g2_font_profont22_mf);      // extended font
    pu8f->setForegroundColor(foreground_color); // apply color
    pu8f->setFontMode(1);                       // use u8g2 transparent mode (this is default)
    pu8f->setCursor(1, 24);                     // start writing at this position

    // try to render only two lines at a time
    // not to redraw the entire screen
    // because when it reaches the end of the screen the typing speed slows down gradually
    // provides the feeling that the system is sluggish
    static int start_line = 0;
    static int total_line_prev = 0;
    int total_line = Editor::getInstance().total_line;
    char **line_position = Editor::getInstance().line_position;
    int *line_length = Editor::getInstance().line_length;

    // determine where the current editing line will be
    // when reached the max line
    // refresh the screen to the 2nd line
    if (total_line - start_line >= Editor::getInstance().rows)
    {
        // very start of the writing, there is nothing to show
        // so line starts from the first row
        start_line = total_line - 2;
        if (start_line < 0)
            start_line = 0;

        //
        clear_background = true;
    }

    // with backspace, reaches the first character of the screen
    // means there will be no more character displayed on the screen
    if (total_line_prev > total_line && total_line == start_line - 1)
    {
        app_log("%d %d %d\n", total_line_prev, total_line, start_line);

        // clear screen
        clear_background = true;

        start_line -= Editor::getInstance().rows - 1;
        if (start_line < 0)
            start_line = 0;
    }

    // when deleting a word it can be that it is a long word
    // and goes back to the previous screen
    // this case start_line should be redefined
    if (abs(total_line - total_line_prev) > 2 && total_line_prev != 0)
    {
        start_line = total_line - 2;
        if (start_line < 0)
            start_line = 0;

        //
        clear_background = true;
    }

    // if the line has changed then clear the previous line
    //
    if (total_line_prev != total_line)
    {
        // update the flag
        total_line_prev = total_line;
    }

    if (!cleared_background)
    {
        for (int i = start_line + 1; i < total_line - 2; i++)
        {
            // print new line
            pu8f->println("");
        }
    }

    int buffer_start_line = max(total_line - 2, start_line);
    if (cleared_background)
        buffer_start_line = start_line;
    for (int i = buffer_start_line; i <= total_line; i++)
    {
        // print new line
        if (i != start_line)
            pu8f->println("");

        // partial refresh
        if (total_line - i > 2 && total_line > LINE_MAX)
        {
            continue;
        }

        //
        if (line_position[i] != nullptr)
        {
            //
            if (line_position[i] == nullptr)
                break;

            //
            int length = line_length[i];

            // render
            for (int j = 0; j < length; j++)
            {
                // convert extended ascii into a streamlined string
                uint8_t value = *(line_position[i] + j);
                pu8f->print((char)value);
            }
        }
    }
}

void WP_render_blink(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    static bool blink = false;
    static unsigned int last = millis();
    if (millis() - last > 500)
    {
        last = millis();
        blink = !blink;
    }

    int cursorX = pu8f->getCursorX();
    int cursorY = pu8f->getCursorY();

    JsonDocument &app = app_status();
    uint16_t background_color = app["config"]["background_color"].as<uint16_t>();
    uint16_t foreground_color = app["config"]["foreground_color"].as<uint16_t>();

    // delete the previous cursor lines
    static int length_prev = 0;
    static int total_line_prev = 0;
    int total_line = Editor::getInstance().total_line;
    int length = Editor::getInstance().line_length[total_line];

    // draw the blink cursor
    if (blink)
    {
        ptft->drawLine(cursorX + 2, cursorY + 2, cursorX + 12, cursorY + 2, foreground_color);
    }
    else
    {
        ptft->drawLine(cursorX + 2, cursorY + 2, cursorX + 12, cursorY + 2, background_color);
    }

    if (total_line != total_line_prev)
    {

        // when going back one line above with backspace
        if (total_line < total_line_prev)
        {
            // delete the line below
            ptft->fillRect(0, cursorY, 320, 60, background_color);

            // delete the last character of the line
            ptft->fillRect(cursorX - 20, cursorY - 22, 320, 24, background_color);
        }

        // word wrap occurred
        if (length > 0)
        {
            // delete the last part of the previous line
            ptft->fillRect(320 - 12 * (length + 1), cursorY - 40, 320, 24, background_color);
        }

        // line changed remove the previous line trail
        // delete the previous trails
        ptft->drawLine(
            0,
            cursorY - 16,
            screen_width,
            cursorY - 16,
            background_color);
        //
        total_line_prev = total_line;
        //
        blink = true;
        last = millis();
    }

    if (length_prev != length)
    {
        // handle backspace
        if (length_prev > length)
        {
            // delete the character
            ptft->fillRect(cursorX, cursorY - 17, 320, 40, background_color);
        }

        // delete the previous trails
        ptft->drawLine(
            cursorX - (20 * (length - length_prev)),
            cursorY + 2,
            cursorX + 24,
            cursorY + 2,
            background_color);

        //
        length_prev = length;

        //
        blink = true;
        last = millis();
    }
}

void WP_render_clear(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    // check if the editor requests to clear screen
    if (Editor::getInstance().clear)
    {
        Editor::getInstance().clear = false;
        clear_background = true;
    }

    if (clear_background)
    {
        // reset flag
        clear_background = false;
        cleared_background = true;

        // apply background color
        JsonDocument &app = app_status();
        uint16_t background_color = app["config"]["background_color"].as<uint16_t>();
        uint16_t foreground_color = app["config"]["foreground_color"].as<uint16_t>();

        // clear screen
        ptft->fillScreen(background_color);

#ifdef ENV_USBHOST
        // write keyboard layout
        String layout = app["config"]["keyboard_layout"].as<String>();
        if (layout == "null" || layout.isEmpty())
            layout = "US"; // defaults to US layout
        // draw status bar
        ptft->setCursor(280, STATUSBAR_Y, 2);
        ptft->setTextColor(foreground_color, background_color);
        ptft->setTextSize(1);
        ptft->print(layout);
#endif
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

// Check if text is saved
void WP_check_saved()
{
    static unsigned int last = millis();
    static unsigned int text_pos_prev = Editor::getInstance().text_pos;
    unsigned int text_pos = Editor::getInstance().text_pos;

    // extend the auto save timer
    if (text_pos_prev != text_pos)
    {
        text_pos_prev = text_pos;
        last = millis();
    }

    // when idle for 5 seconds then auto save
    if (millis() - last > 5000)
    {
        last = millis();
        Editor::getInstance().saveFile();
    }
}

void WP_check_sleep()
{
    static unsigned int text_pos_prev = Editor::getInstance().text_pos;
    unsigned int text_pos = Editor::getInstance().text_pos;

    if (text_pos_prev != text_pos)
    {
        // when typed then reset sleep timer
        text_pos_prev = text_pos;
        last_sleep = millis();
    }

    // 600 seconds - 10 minutes
    if (millis() - last_sleep > 600000)
    {
        // if no action for 10 minute go to sleep
        last_sleep = -1;

        //
        JsonDocument &app = app_status();
        app["screen"] = SLEEPSCREEN;
    }
}