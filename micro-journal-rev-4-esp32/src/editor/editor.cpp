#include "editor.h"
#include "app/app.h"
#include "SD/sd.h"
#include "display/display.h"
#include "keyboard/keyboard.h"
#include "keyboard/ascii/ascii.h"

#include <FS.h>
#include <SD.h>
#include <algorithm>

// Setup function
void Editor::loadFile(String fileName)
{
    if (this->saving)
    {
        // file operation on-going
        app_log("File operation on-going\n");
        return;
    }

    // save to global
    this->fileName = fileName;

    // Ensure the saving flag is reset at the end of the function
    this->saving = true;
    auto cleanup = [this]()
    {
        this->saving = false;
    };

    // Check SD card status
    JsonDocument &app = app_status();

    // Check if the file exists, create if not
    if (!SD.exists(fileName))
    {
        File file = SD.open(fileName, FILE_WRITE);
        if (!file)
        {
            //
            file.close();
            delay(100);

            //
            app["error"] = "Failed to create file";
            app["screen"] = ERRORSCREEN;

            //
            cleanup();

            return;
        }

        //
        file.close();
        delay(100);

        //
        app_log("File created\n");
    }

    // open the text file
    app_log("Loading file %s\n", fileName);
    File file = SD.open(fileName);
    if (!file)
    {
        //
        app["error"] = format("file open failed %s\n", fileName);
        app["screen"] = ERRORSCREEN;
        app_log(app["error"].as<const char *>());

        //
        cleanup();

        return;
    }

    // Determine file size and set buffer accordingly
    fileSize = file.size();

    // calcualte the file offset
    int offset = 0;
    if (fileSize > 0)
        offset = (fileSize / BUFFER_SIZE) * BUFFER_SIZE;
    if (fileSize - offset == 0 && offset > 0)
    {
        // when it is exactly the buffer end
        // go one buffer behind
        offset -= BUFFER_SIZE;
        if (offset < 0)
            offset = 0;
    }
    if (!file.seek(offset))
    {
        //
        file.close();
        delay(100);

        //
        app["error"] = format("Failed to seek file pointer. fileSize: %d offset: %d\n", fileSize, offset);
        app["screen"] = ERRORSCREEN;
        app_log(app["error"].as<const char *>());

        //
        cleanup();

        return;
    }

    //
    app_log("Reading file from: %d of size: %d\n", offset, fileSize - offset);

    // reset the buffer
    memset(buffer, '\0', sizeof(buffer));

    // Read file content into text buffer
    int pos = 0;
    buffer[pos] = '\0';
    while (file.available())
    {
        buffer[pos++] = file.read();
    }
    text_pos = pos;

    //
    file.close();
    delay(100);

    app_log("File loaded: text_pos: %d, fileSize: %d\n",
            text_pos,
            fileSize);

    // calculate screen buffer
    calculate_screen_buffer();

    //
    this->clear = true;
    this->saved = true;

    //
    cleanup();
}

void Editor::saveFile()
{
    if (this->saved || this->saving)
    {
        return;
    }

    // set saving flag
    this->saving = true;
    // Ensure the saving flag is reset at the end of the function
    auto cleanup = [this]()
    {
        this->saving = false;
    };

    //
    JsonDocument &app = app_status();

    //
    app_log("%s\n", fileName);
    File file = SD.open(fileName, FILE_WRITE);
    if (!file)
    {
        //
        app["error"] = "Failed to open file for writing\n";
        app["screen"] = ERRORSCREEN;
        app_log(app["error"].as<const char *>());

        //
        cleanup();

        return;
    }

    // Seek to the specified offset
    int offset = 0;
    if (fileSize > 0)
        offset = (fileSize / BUFFER_SIZE) * BUFFER_SIZE;
    if (!file.seek(offset))
    {
        app_log("Failed to seek file pointer\n");
        file.close();
        delay(100);

        //
        cleanup();

        return;
    }
    app_log("Writing file at: %d\n", offset);

    // writing the file content
    size_t length = file.print(buffer);
    if (length >= 0)
    {
        app_log("File written: %d bytes\n", length);
    }
    else
    {
        app["error"] = "Save failed\n";
        app["screen"] = ERRORSCREEN;
        app_log(app["error"].as<const char *>());
    }

    //
    file.close();
    delay(100);

    // recalculate the file size
    // calculate the file size
    file = SD.open(fileName, FILE_READ);
    if (!file)
    {
        app["error"] = "Failed to open file for reading\n";
        app_log(app["error"].as<const char *>());
        app["screen"] = ERRORSCREEN;

        //
        cleanup();

        //
        return;
    }
    fileSize = file.size();
    file.close();
    delay(100);

    app_log("File size: %d, text_pos: %d\n", fileSize, text_pos);

    // file is saved
    this->saved = true;

    //
    cleanup();
}

void Editor::clearFile()
{
    if (this->saving)
    {
        // file operation on-going
        app_log("File operation on-going\n");
        return;
    }

    // set saving flag
    this->saving = true;
    // Ensure the saving flag is reset at the end of the function
    auto cleanup = [this]()
    {
        this->saving = false;
    };

    //
    JsonDocument &app = app_status();

    // Rename the existing file and create a new one
    char newName[100];
    snprintf(newName, sizeof(newName), "/%s_backup.txt", fileName);
    if (SD.exists(newName))
    {
        SD.remove(newName); // remove the backup file
    }

    if (SD.rename(fileName, newName))
    {
        app_log("File renamed successfully: %s.\n", newName);
    }
    else
    {
        app["error"] = format("Error renaming file. %s\n", newName);
        app_log(app["error"].as<const char *>());
        app["screen"] = ERRORSCREEN;

        cleanup();

        return;
    }

    File file = SD.open(fileName, FILE_WRITE);
    if (!file)
    {
        app["error"] = format("Failed to create file. %s\n", fileName);
        app_log(app["error"].as<const char *>());
        app["screen"] = ERRORSCREEN;

        cleanup();

        return;
    }
    file.close();
    delay(100);

    //
    cleanup();

    // reload the file
    loadFile(fileName);
}

String Editor::getFileSize()
{
    size_t num = fileSize + text_pos;
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

void Editor::keyboard(char key)
{
    if (this->saving)
    {
        // do not handle key inputs when file operation is on-going
        app_log("Skip inputs File operation on-going\n");
        return;
    }
    // every key stroke saved = false;
    this->saved = false;

    // BACKSPACE
    if (key == '\b')
    {
        if (text_pos > 0)
        {
            // go backwards
            buffer[--text_pos] = '\0';
        }
        else
        {
            // Load previous contents from the file if at the beginning of the buffer
            app_log("Backspace reached the beginning of the buffer\n");

            //
            text_pos = 0;
            buffer[0] = '\0';

            //
            saveFile();

            //
            loadFile(this->fileName);

            //
        }
    }

    // DEL key deletes the word
    else if (key == 127)
    {
        delete_word();
    }

    // FORWARD TYPING
    else
    {
        buffer[text_pos++] = key;
        buffer[text_pos] = '\0';
    }

    // WHEN BUFFER IS FULL
    if (text_pos >= BUFFER_SIZE)
    {
        app_log("Text buffer full\n");

        //
        saveFile();

        //
        loadFile(this->fileName);
    }

    // update the screen buffer
    calculate_screen_buffer();
}

void Editor::calculate_screen_buffer()
{
    // Initialize the screen text lines
    this->total_line = 0;
    int row_character_count = 0;
    int last_space_index = -1;
    int last_space_position = -1;

    line_position[0] = &buffer[0];
    line_length[0] = cols;

    for (int i = 0; i < BUFFER_SIZE; i++)
    {
        // When reaching the end of text, break
        if (buffer[i] == '\0')
        {
            // Update the length of the last line
            line_length[total_line] = row_character_count;
            // Mark the end of the text
            line_position[total_line + 1] = nullptr;
            line_length[total_line + 1] = 0;

            //
            break;
        }

        // Count total characters in a line
        row_character_count++;

        // Track the position of the last space
        if (buffer[i] == ' ')
        {
            last_space_index = i;
            last_space_position = row_character_count;
        }

        // When receiving a newline or max characters reached, start a new line
        if (buffer[i] == '\n' || row_character_count == cols)
        {
            if (buffer[i] == '\n')
            {
                // Handle newline character
                total_line++;
                line_position[total_line] = &buffer[i + 1];

                // new line is not included in the line
                line_length[total_line - 1] = row_character_count - 1;

                // reset counters
                row_character_count = 0;
                last_space_index = -1;
                last_space_position = -1;
            }
            else if (last_space_index != -1 && buffer[i] != '\n')
            {
                // If there's a space within the line, wrap at the last space
                int wrap_position = last_space_position;
                total_line++;
                line_position[total_line] = &buffer[last_space_index + 1];
                line_length[total_line - 1] = wrap_position;

                //
                row_character_count -= wrap_position;
                last_space_index = -1;
                last_space_position = -1;
            }
            else
            {
                // If no spaces to wrap at, wrap at the max character limit
                total_line++;
                line_position[total_line] = &buffer[i + 1];
                line_length[total_line - 1] = row_character_count;
                row_character_count = 0;
            }
        }
    }
}

void Editor::delete_word()
{
    int length = text_pos;
    if (length == 0)
        return;

    // Find the position of the last non-space character
    int end = length - 1;
    while (end >= 0 && buffer[end] == ' ')
        end--;

    // If buffer is all spaces or empty
    if (end < 0)
        return;

    // Find the beginning of the last word
    int start = end;
    while (start >= 0 && buffer[start] != ' ' && buffer[start] != '\n')
        start--;

    app_log("%d\n", start);
    // add space at the last stop
    if(start < 0) {
        start = 0;
        buffer[0] = '\0';
    }
    buffer[start] = ' ';

    // Remove the last word and any trailing spaces
    buffer[start + 1] = '\0';

    // reset the text pos
    text_pos = start + 1;

    // recalculate the screen buffer
    calculate_screen_buffer();
}