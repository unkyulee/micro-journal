#include "editor.h"
#include "app/app.h"
#include "display/display.h"

//
#include <FS.h>
#include <SD.h>

// Given the fileName, go through the loading process
// initialize FileBuffer and ScreenBuffer
void Editor::loadFile(String fileName)
{
    // app status
    JsonDocument &app = app_status();

    // if no current file is specified then skip
    if (fileName.isEmpty())
    {
        app["error"] = format("Filename is empty. Can't load.\n");
        app_log(app["error"].as<const char *>());
        app["screen"] = ERRORSCREEN;

        return;
    }

    // Step 1. Create file if necessary
    if (!SD.exists(fileName))
    {
        File file = SD.open(fileName, FILE_WRITE);
        if (!file)
        {
            app["error"] = format("Failed to create a file. %s\n", fileName);
            app_log(app["error"].as<const char *>());
            app["screen"] = ERRORSCREEN;

            return;
        }

        //
        file.close();
        delay(100);

        //
        app_log("File created. %s\n", fileName);
    }

    // Step 2. Initialize the FileBuffer
    fileBuffer.load(fileName, 0);

    // Update the Screen Buffer
    screenBuffer.Update(fileBuffer);
    screenBuffer.clear = true;
}

//
void Editor::saveFile()
{
    //
    JsonDocument &app = app_status();

    // if no current file is specified then skip
    if (fileBuffer.getFileName().isEmpty())
    {
        app["error"] = format("Filename is empty. Can't save.\n");
        app_log(app["error"].as<const char *>());
        app["screen"] = ERRORSCREEN;

        return;
    }

    // if already save nothing to do
    if (this->saved)
    {
        app_log("File already saved\n");
        return;
    }

    //
    fileBuffer.save();

    // flag to save
    this->saved = true;
}

// Make the current file empty
void Editor::clearFile()
{
    //
    JsonDocument &app = app_status();

    // if no current file is specified then skip
    String fileName = fileBuffer.getFileName();
    if (fileName.isEmpty())
    {
        app["error"] = format("Filename is empty. Can't clear.\n");
        app_log(app["error"].as<const char *>());
        app["screen"] = ERRORSCREEN;

        return;
    }

    // Step 1. Check if the backup file exists
    // remove it if already exists
    String backupFileName = format("/%s_backup.txt", fileBuffer.getFileName());
    if (SD.exists(backupFileName))
    {
        // remove the backup file
        SD.remove(backupFileName);
    }

    // Step 2. Rename the current file to the backup.txt
    if (SD.rename(fileBuffer.getFileName(), backupFileName))
    {
        app_log("File renamed successfully: %s.\n", backupFileName);
    }
    else
    {
        app["error"] = format("Error making a backup file. %s\n", backupFileName);
        app_log(app["error"].as<const char *>());
        app["screen"] = ERRORSCREEN;

        return;
    }

    // Step 3. Empty the current file by opening it with FILE_WRITE
    File file = SD.open(fileName, FILE_WRITE);
    if (!file)
    {
        app["error"] = format("Failed to create an empty file %s\n", fileName);
        app_log(app["error"].as<const char *>());
        app["screen"] = ERRORSCREEN;

        return;
    }

    // clean up file
    file.close();
    delay(100);

    // Step 4. Go through the loading process of the empty file
    loadFile(fileName);
}

// Handle Keyboard Input
void Editor::keyboard(char key)
{
    // when ever keyboard event occurs
    // set saved flag to false
    this->saved = false;

    //////////////////////////
    // BACKWARD EDITING
    //////////////////////////
    if (key == '\b')
    {
        // buffer has more than 1 character
        if (fileBuffer.getBufferSize() > 0)
        {
            fileBuffer.removeLastChar();
        }
        // buffer emptied
        else
        {
            // Load previous contents from the file if at the beginning of the buffer
            app_log("Backspace reached the beginning of the buffer\n");

            //
            saveFile();

            //
            loadFile(fileBuffer.fileName);
        }
    }

    // DEL key deletes the word
    else if (key == 127)
    {
        if (fileBuffer.getBufferSize() > 0)
        {
            fileBuffer.removeLastWord();
        }
        // buffer emptied
        else
        {
            // Load previous contents from the file if at the beginning of the buffer
            app_log("Delete word reached the beginning of the buffer\n");

            //
            saveFile();

            //
            loadFile(fileBuffer.fileName);
        }
    }

    //////////////////////////
    // CURSORS
    //////////////////////////
    else if (key >= 18 && key <= 23 || key == 2 || key == 3)
    {
        // arrow keys
        // 18 - Left, 19 - Right, 20 - Up, 21 - Down
        // 22 - Page Up, 23 - Page Down
        // 2 - Home 3 - End
        if(key == 18) {
            // left
            --fileBuffer.cursorPos;
        } else if(key == 19) {
            ++fileBuffer.cursorPos;
        }
        app_log("Cursor Pos: %d\n", fileBuffer.cursorPos);
    }

    //////////////////////////
    // FORWARD EDITING
    //////////////////////////
    else
    {
        // add to the edit buffer new character
        if (!fileBuffer.available())
        {
            app_log("Text buffer full\n");

            //
            saveFile();

            //
            loadFile(fileBuffer.fileName);
        }

        //
        fileBuffer.addChar(key);
    }

    // update the screen buffer
    screenBuffer.Update(fileBuffer);
}
