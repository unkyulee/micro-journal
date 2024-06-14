#include "WordProcessor.h"
#include "app/app.h"
#include "SD/sd.h"
#include "display/display.h"
#include "keyboard/keyboard.h"
#include "keyboard/ascii/ascii.h"

#include <FS.h>
#include <SD.h>
#include <algorithm>

// Initialize static instance
WordProcessor *WordProcessor::instance = nullptr;

// Get instance of WordProcessor
WordProcessor &WordProcessor::getInstance(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    if (!instance)
    {
        instance = new WordProcessor(ptft, pu8f);
    }
    return *instance;
}

// Setup function
void WordProcessor::setup()
{
    //
    // Check SD card status
    JsonDocument &app = app_status();

    // Check if the file exists, create if not
    if (!SD.exists(FILENAME))
    {
        File file = SD.open(FILENAME, FILE_WRITE);
        if (!file)
        {
            //
            Serial.println("Failed to create file");

            return;
        }
        file.close();
        Serial.println("File created");
    }

    // setup default color
    if(!app["config"].containsKey("foreground_color")) {
        app["config"]["foreground_color"] = TFT_WHITE;
    }

    // Load text from file
    loadText();

    // Clear background
    clear = true;

    // sleep timer reset
    last_sleep = millis();
}

// Render text on screen
void WordProcessor::render()
{
    bool cleared = clearBackground();
    clearTrails();
    checkSaved();
    checkSleep();

    JsonDocument &app = app_status();
    uint16_t background_color = app["config"]["background_color"].as<uint16_t>();
    uint16_t foreground_color = app["config"]["foreground_color"].as<uint16_t>();

    // Calculate the screen text lines
    // try to display from the second line when possible
    // then when cursor reaches the end refresh the screen
    int total_line = 0;
    int row_character_count = 0;
    char last_char;

    line_position[0] = &text_buffer[0];
    line_length[0] = MAX_ROW_CHARACTERS;

    for (int i = 0; i < TEXT_BUFFER_SIZE; i++)
    {
        // when reaching end of text
        // break
        if (text_buffer[i] == '\0')
        {
            //
            line_length[total_line] = row_character_count;

            //
            line_position[total_line + 1] = nullptr;
            line_length[total_line + 1] = 0;

            // debug message
            // app_log("%d %d %d\n", total_line, line_length[total_line]);

            //
            break;
        }

        // count total characters in a line
        row_character_count++;

        // when receiving a \n or max character reached
        // start a new line
        if (text_buffer[i] == '\n' || row_character_count == MAX_ROW_CHARACTERS)
        {
            //
            total_line += 1;

            //
            line_position[total_line] = &text_buffer[i + 1];
            line_length[total_line - 1] = row_character_count;

            // if last character is new line don't count in
            if (text_buffer[i] == '\n')
            {
                line_length[total_line - 1] = row_character_count - 1;
            }

            //
            row_character_count = 0;
        }

        //
        last_char = text_buffer[i];
    }

    // determine where the current editing line will be
    int start_line = start_line_prev;

    // when reached the max line
    // refresh the screen to the 2nd line
    if (total_line - start_line_prev >= MAX_LINES)
    {
        // very start of the writing, there is nothing to show
        // so line starts from the first row
        start_line = total_line - 2;
        if (start_line < 0)
            start_line = 0;

        // refresh the screen
        clear = true;
        start_line_prev = start_line;
    }

    // with backspace, reaches the first character of the screen
    // means there will be no more character displayed on the screen
    if (total_line_prev > total_line && total_line == start_line - 1)
    {
        app_log("%d %d %d\n", total_line_prev, total_line, start_line);

        // clear screen
        clear = true;

        start_line -= MAX_LINES - 1;
        if (start_line < 0)
            start_line = 0;

        start_line_prev = start_line;
    }

    // if the line has changed then clear the previous line
    //
    if (total_line_prev != total_line)
    {
        // update the flag
        total_line_prev = total_line;
    }

    // STATUS BAR
    ptft->setCursor(0, STATUSBAR_Y, 2);
    ptft->setTextColor(foreground_color, background_color);
    ptft->setTextSize(1);
    ptft->printf("%s bytes", formatNumberWithCommas(fileSize + text_pos - text_last_save_pos));
    /*
    ptft->printf("%s, %d, %d, %d, %d, %d",
                 formatNumberWithCommas(fileSize + text_pos - text_last_save_pos),
                 text_pos,
                 text_last_save_pos,
                 total_line,
                 start_line,
                 line_length[total_line - 1]);
    */
    if (text_pos == text_last_save_pos)
    {
        ptft->fillCircle(310, STATUSBAR_Y + 8, 5, TFT_GREEN);
    }
    else
    {
        ptft->fillCircle(310, STATUSBAR_Y + 8, 5, TFT_RED);
    }
    ptft->drawCircle(310, STATUSBAR_Y + 8, 5, TFT_BLACK);

    /////
    // Render the user text
    pu8f->setFont(u8g2_font_profont22_mf); // extended font
    pu8f->setForegroundColor(foreground_color);   // apply color
    pu8f->setFontMode(1);                  // use u8g2 transparent mode (this is default)
    pu8f->setCursor(0, 24);                // start writing at this position

    // try to render only two lines at a time
    // not to redraw the entire screen
    // because when it reaches the end of the screen the typing speed slows down gradually
    // provides the feeling that the system is sluggish

    // to prevent this. Just skips the previous lines that are already rendered
    if (!cleared)
    {
        for (int i = start_line + 1; i < total_line - 2; i++)
        {
            // print new line
            pu8f->println("");
        }
    }

    int buffer_start_line = max(total_line - 2, start_line);
    if (cleared)
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
                if (value < 128)
                {
                    pu8f->print((char)value);
                }
                else
                {
                    pu8f->print(ascii_convert_unicode(value));
                }
            }
        }
    }

    /////
    blinkCursor();
}

// Empty the file
void WordProcessor::emptyFile()
{
    // Rename the existing file and create a new one
    char newName[30];
    snprintf(newName, sizeof(newName), "/%lu.txt", millis());

    if (SD.rename(FILENAME, newName))
    {
        app_log("File renamed successfully: %s.\n", newName);
    }
    else
    {
        app_log("Error renaming file. %s\n", newName);
        return;
    }

    File file = SD.open(FILENAME, FILE_WRITE);
    if (!file)
    {
        Serial.println("Failed to create file");
        JsonDocument &app = app_status();
        app["screen"] = ERRORSCREEN;
        return;
    }
    file.close();
    app_log("File created\n");

    // after write operation make sure to give some delay for safe operation
    delay(250);
}

// Load text from file
void WordProcessor::loadText()
{
    File file = SD.open(FILENAME);
    if (!file)
    {
        app_log("Failed to open file for reading\n");
        JsonDocument &app = app_status();
        app["screen"] = ERRORSCREEN;
        return;
    }

    // Determine file size and set buffer accordingly
    fileSize = file.size();

    // calcualte the file offset
    int offset = 0;
    if (fileSize > 0)
        offset = (fileSize / TEXT_BUFFER_SIZE) * TEXT_BUFFER_SIZE;
    if (!file.seek(offset))
    {
        app_log("Failed to seek file pointer\n");
        file.close();
        return;
    }
    app_log("Reading file from: %d\n", offset);

    // Read file content into text buffer
    int pos = 0;
    text_buffer[pos] = '\0';
    while (file.available())
    {
        text_buffer[pos++] = file.read();
        text_buffer[pos] = '\0';
    }
    text_pos = pos;
    text_pos_prev = pos;
    text_last_save_pos = pos;
    start_line_prev = 0;

    app_log("File loaded: text_pos: %d, fileSize: %d\n",
            text_pos,
            fileSize);

    file.close();
    delay(100);
}

// Save text to file
void WordProcessor::saveText()
{
    if (text_last_save_pos == text_pos)
    {
        // No changes, no need to save
        app_log("Nothing is changed\n");
        return;
    }

    static unsigned int last = 0;
    if (millis() - last > 1000)
    {
        last = millis();
        File file = SD.open(FILENAME, FILE_WRITE);
        if (!file)
        {
            app_log("Failed to open file for writing\n");
            JsonDocument &app = app_status();
            app["screen"] = ERRORSCREEN;
            return;
        }

        // Seek to the specified offset
        int offset = 0;
        if (fileSize > 0)
            offset = (fileSize / TEXT_BUFFER_SIZE) * TEXT_BUFFER_SIZE;
        if (!file.seek(offset))
        {
            app_log("Failed to seek file pointer\n");
            file.close();
            return;
        }
        app_log("Writing file at: %d\n", offset);

        // writing the file content
        size_t length = file.print(text_buffer);
        if (length >= 0)
        {
            app_log("File written: %d bytes\n", length);
            //
            text_last_save_pos = text_pos;
        }
        else
        {
            app_log("Write failed\n");
            JsonDocument &app = app_status();
            app["screen"] = ERRORSCREEN;
        }

        //
        file.close();

        // save operation takes time before loading is available
        delay(100);

        // recalculate the file size
        // calculate the file size
        file = SD.open(FILENAME, FILE_READ);
        if (!file)
        {
            app_log("Failed to open file for reading\n");
            JsonDocument &app = app_status();
            app["screen"] = ERRORSCREEN;
            return;
        }
        fileSize = file.size();
        file.close();

        // save operation takes time before loading is available
        delay(100);

        app_log("File size: %d, text_pos: %d\n", fileSize, text_pos);
    }
}

// Handle keyboard input
void WordProcessor::keyboard(char key)
{
    if (file_on_going)
    {
        app_log("Skip Key Press. File Operation OnGonig.\n");
        return;
    }

    // Check if menu key is pressed
    if (key == MENU)
    {
        // Save before transitioning to the menu
        file_on_going = true;
        saveText();
        file_on_going = false;

        //
        JsonDocument &app = app_status();
        app["screen"] = MENUSCREEN;
        return;
    }

    ///
    // handling BACKSPACE
    if (key == '\b')
    {
        if (text_pos > 0)
        {
            text_buffer[--text_pos] = '\0';
        }
        else
        {
            // Load previous contents from the file if at the beginning of the buffer
            app_log("Backspace reached the beginning of the buffer\n");
            clear = true;
            text_pos = 0;
            text_buffer[0] = '\0';

            file_on_going = true;
            saveText();
            loadText();
            file_on_going = false;
        }
    }
    ///

    else
    {
        text_buffer[text_pos++] = key;
        text_buffer[text_pos] = '\0';
    }

    //
    if (text_pos >= TEXT_BUFFER_SIZE)
    {
        app_log("Text buffer full\n");

        file_on_going = true;
        saveText();
        loadText();
        file_on_going = false;

        // when the buffer is about to finish try to save
        clear = true;
    }
}

// Clear background
bool WordProcessor::clearBackground()
{
    if (clear)
    {
        //
        clear = false;

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
#endif

        // draw status bar
        ptft->setCursor(280, STATUSBAR_Y, 2);
        ptft->setTextColor(foreground_color, background_color);
        ptft->setTextSize(1);
        ptft->print(layout);

        return true;
    }

    return false;
}

// Clear trails
void WordProcessor::clearTrails()
{
    if (text_pos_prev != text_pos)
    {
        // get current cursor position
        int cursorX = pu8f->getCursorX();
        int cursorY = pu8f->getCursorY();

        JsonDocument &app = app_status();
        uint16_t background_color = app["config"]["background_color"].as<uint16_t>();
        uint16_t foreground_color = app["config"]["foreground_color"].as<uint16_t>();

        // remove previous cursor
        for (int i = 0; i <= text_pos - text_pos_prev; i++)
        {
            ptft->drawLine(cursorX + 2 - (10 * (i + 1)), cursorY + 2, cursorX + 24 - (10 * (i + 1)), cursorY + 2, background_color);
        }

        // in case when back space is pressed
        if (text_pos_prev > text_pos)
        {
            // delete the character
            ptft->fillRect(cursorX - 12, cursorY - 17, 320, 40, background_color);
        }

        // always show the cursor when typing
        blink = true;

        // update the text_pos
        text_pos_prev = text_pos;
    }
}

// Blink Cursor
void WordProcessor::blinkCursor()
{
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

    //
    if (blink)
    {
        ptft->drawLine(cursorX + 2, cursorY + 2, cursorX + 12, cursorY + 2, foreground_color);
    }
    else
    {
        ptft->drawLine(cursorX + 2, cursorY + 2, cursorX + 12, cursorY + 2, background_color);
    }
}

// Check if text is saved
void WordProcessor::checkSaved()
{
    static unsigned int last = millis();
    static unsigned int text_pos_last = -1;
    if (text_pos_last != text_pos)
    {
        text_pos_last = text_pos;
        last = millis();
    }

    // when idle for 5 seconds then auto save
    if (millis() - last > 5000)
    {
        last = millis();
        if (text_pos != text_last_save_pos)
        {
            file_on_going = true;
            saveText();
            file_on_going = false;
        }
    }
}

//
void WordProcessor::checkSleep()
{
    static unsigned int text_pos_last = -1;
    if (text_pos_last != text_pos)
    {
        // when typed then reset sleep timer
        text_pos_last = text_pos;
        last_sleep = millis();
    }

    if (millis() - last_sleep > 600000)
    {
        // if no action for 10 minute go to sleep
        last_sleep = -1;

        //
        JsonDocument &app = app_status();
        app["screen"] = SLEEPSCREEN;
    }
}

// Format number with commas
String WordProcessor::formatNumberWithCommas(long num)
{
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

    return formattedNumber;
}
