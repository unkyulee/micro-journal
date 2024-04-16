#include "WordProcessor.h"
#include "app/app.h"
#include "app/SD/sd.h"
#include "service/display/display.h"
#include "service/keyboard/keyboard.h"

#include <FS.h>
#include <SD.h>

// Initialize static instance
WordProcessor *WordProcessor::instance = nullptr;

// Get instance of WordProcessor
WordProcessor &WordProcessor::getInstance(TFT_eSPI *ptft)
{
    if (!instance)
    {
        instance = new WordProcessor(ptft);
    }
    return *instance;
}

// Setup function
void WordProcessor::setup()
{
    //
    // Check SD card status
    JsonDocument &app = app_status();
    if (app["SD"].as<bool>() == false)
    {
        app_log("SD card is not ready. Not initializing WordProcessor\n");
        app["screen"] = ERRORSCREEN;
        return;
    }

    // Check if the file exists, create if not
    if (!SD.exists(FILENAME))
    {
        File file = SD.open(FILENAME, FILE_WRITE);
        if (!file)
        {
            Serial.println("Failed to create file");
            return;
        }
        file.close();
        Serial.println("File created");
    }

    // Load text from file
    loadText();

    // Clear background
    clear = true;
}

// Render text on screen
void WordProcessor::render()
{
    clearBackground();
    clearTrails();
    checkSaved();

    int total_line = 0;
    int row_character_count = 0;
    char last_char;

    line_position[0] = &text_buffer[0];
    for (int i = 0; i < TEXT_BUFFER_SIZE; i++)
    {
        if (text_buffer[i] == '\0')
            break;

        if (row_character_count == 0 && text_buffer[i] == ' ')
        {
            line_position[total_line] = &text_buffer[i + 1];
            continue;
        }

        row_character_count++;

        if (text_buffer[i] == '\n' || row_character_count == MAX_ROW_CHARACTERS)
        {
            line_position[++total_line] = &text_buffer[i + 1];
            row_character_count = 0;
        }

        last_char = text_buffer[i];
    }

    int start_line = start_line_prev;
    if (total_line - start_line_prev >= MAX_LINES)
    {
        start_line = total_line - 2;
        if (start_line < 0)
            start_line = 0;

        clear = true;

        start_line_prev = start_line;
    }

    if (total_line_prev != total_line)
    {
        total_line_prev = total_line;
    }

    ptft->setCursor(0, STATUSBAR_Y, 2);
    ptft->setTextColor(TFT_WHITE, STATUSBAR_COLOR);
    ptft->setTextSize(1);
    ptft->printf(" %s bytes", formatNumberWithCommas(fileSize + text_pos - text_last_save_pos));
    if (text_pos == text_last_save_pos)
    {
        ptft->fillCircle(310, STATUSBAR_Y + 8, 5, TFT_GREEN);
    }
    else
    {
        ptft->fillCircle(310, STATUSBAR_Y + 8, 5, TFT_RED);
    }
    ptft->drawCircle(310, STATUSBAR_Y + 8, 5, TFT_BLACK);

    ptft->setFreeFont(&FreeMono9pt7b);
    ptft->setCursor(0, 36);
    ptft->setTextColor(TFT_WHITE, TFT_BLACK);
    ptft->setTextSize(1);
    ptft->print(line_position[start_line]);

    if (row_character_count == 0 && total_line > 0 && last_char != '\n')
    {
        ptft->println("");
    }

    blinkCarrot();
}

// Load text from file
void WordProcessor::loadText()
{
    app_log("Reading file: %s ... ", FILENAME);
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
    if (fileSize > TEXT_BUFFER_SIZE / 2)
    {
        if (!file.seek(-TEXT_BUFFER_SIZE / 2, SeekEnd))
        {
            app_log("Failed to seek file pointer\n");
            JsonDocument &app = app_status();
            app["screen"] = ERRORSCREEN;
            file.close();
            return;
        }
        text_last_save_pos = TEXT_BUFFER_SIZE / 2;
    }
    else
    {
        text_last_save_pos = fileSize;
    }

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
    start_line_prev = 0;

    app_log("File loading completed: text_pos: %d, fileSize: %d\n", text_pos, fileSize);

    file.close();
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

        app_log("Writing file: %s ...", FILENAME);
        File file = SD.open(FILENAME, FILE_WRITE);
        if (!file)
        {
            app_log("Failed to open file for writing\n");
            JsonDocument &app = app_status();
            app["screen"] = ERRORSCREEN;
            return;
        }

        // Seek to the specified offset
        size_t offset = fileSize - (TEXT_BUFFER_SIZE - text_last_save_pos);
        if (fileSize < (TEXT_BUFFER_SIZE - text_last_save_pos))
            offset = 0;
        app_log("file size: %d file offset: %d text_last_save: %d\n", fileSize, offset, text_last_save_pos);
        if (!file.seek(offset))
        {
            app_log("Failed to seek file pointer\n");
            file.close();
            return;
        }

        size_t length = file.print(text_buffer);
        if (length >= 0)
        {
            app_log("File written from %d at length %d\n", offset, length);
            text_last_save_pos = text_pos;
            fileSize += length;
        }
        else
        {
            app_log("Write failed\n");
            JsonDocument &app = app_status();
            app["screen"] = ERRORSCREEN;
        }
        file.close();
    }
}

// Handle keyboard input
void WordProcessor::keyboard(char key)
{
    // Check if menu key is pressed
    if (key == MENU)
    {
        saveText(); // Save before transitioning to the menu
        JsonDocument &app = app_status();
        app["screen"] = MENUSCREEN;
        return;
    }

    if (key == '\b')
    {
        if (text_pos > 10)
        {
            text_buffer[--text_pos] = '\0';
        }
        else
        {
            // Load previous contents from the file if at the beginning of the buffer
            clear = true;
            text_pos = 0;
            text_buffer[0] = '\0';
            saveText();
            delay(100);
            loadText();
        }
    }
    else
    {
        text_buffer[text_pos++] = key;
        text_buffer[text_pos] = '\0';
    }

    if (text_pos >= TEXT_BUFFER_SIZE)
    {
        saveText();
        delay(100);
        loadText();
        clear = true;
    }
}

// Clear background
void WordProcessor::clearBackground()
{
    if (clear)
    {
        clear = false;
        ptft->fillScreen(TFT_BLACK);
        ptft->fillRect(0, STATUSBAR_Y, 320, 240, STATUSBAR_COLOR);
    }
}

// Clear trails
void WordProcessor::clearTrails()
{
    if (text_pos_prev != text_pos)
    {
        int cursorX = ptft->getCursorX();
        int cursorY = ptft->getCursorY();
        ptft->drawLine(cursorX, cursorY, cursorX + 10, cursorY, TFT_BLACK);

        if (text_pos_prev > text_pos)
        {
            ptft->fillRect(cursorX - 12, cursorY - 12, 320, 24, TFT_BLACK);
        }

        blink = true;
        text_pos_prev = text_pos;
    }
}

// Blink carrot
void WordProcessor::blinkCarrot()
{
    static unsigned int last = millis();
    if (millis() - last > 500)
    {
        last = millis();
        blink = !blink;
    }

    int cursorX = ptft->getCursorX();
    int cursorY = ptft->getCursorY();
    if (blink)
    {
        ptft->drawLine(cursorX, cursorY, cursorX + 10, cursorY, TFT_WHITE);
    }
    else
    {
        ptft->drawLine(cursorX, cursorY, cursorX + 10, cursorY, TFT_BLACK);
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

    if (millis() - last > 5000)
    {
        last = millis();
        if (text_pos != text_last_save_pos)
        {
            saveText();
        }
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
