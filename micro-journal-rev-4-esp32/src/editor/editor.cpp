#include "editor.h"
#include "app/app.h"
#include "display/display.h"

// EDITOR CLASS IMPLEMENTATION
// File Buffer will handle the file handling situation
// Screen Buffer will handle the position of the cursors and text lines

// Editor Initialization with column and row setup
void Editor::init(int cols, int rows)
{
    // If you have a screenBuffer or similar, initialize it as well
    screenBuffer.cols = cols;
    screenBuffer.rows = rows;

    // You can add any additional setup logic here
    _log("Editor initialized with columns: %d, rows: %d\n", cols, rows);
}

// Given the fileName, go through the loading process
// initialize FileBuffer and ScreenBuffer
void Editor::loadFile(String fileName)
{
    // app status
    JsonDocument &app = status();

    // if no current file is specified then skip
    if (fileName.length() == 0)
    {
        //
        app["error"] = "Filename is empty. Can't load.\n";
        app["screen"] = ERRORSCREEN;

        //
        _log(app["error"]);

        return;
    }

    //
    _log("Editor loading file %s\n", fileName.c_str());

    // Step 1. Create file if necessary
    if (!gfs()->exists(fileName.c_str()))
    {
        _log("Creating an empty file since it's new\n");
        File file = gfs()->open(fileName.c_str(), "w");
        if (!file)
        {
            //
            app["error"] = format("Failed to create a file. %s\n", fileName.c_str());
            app["screen"] = ERRORSCREEN;

            //
            _log(app["error"].as<const char *>());
            
            return;
        }

        //
        file.close();
        delay(100);

        //
        _log("File created. %s\n", fileName.c_str());
    }

    // Step 2. Initialize the FileBuffer
    fileBuffer.load(fileName);

    // Step 3. Update the Screen Buffer
    screenBuffer.Update(fileBuffer);
}

//
void Editor::saveFile()
{
    //
    JsonDocument &app = status();

    // if no current file is specified then skip
    if (fileBuffer.getFileName().length() == 0)
    {
        //
        app["error"] = "Filename is empty. Can't save.\n";
        app["screen"] = ERRORSCREEN;

        //
        _log(app["error"]);
        
        return;
    }

    // if already save nothing to do
    if (this->saved)
    {
        _log("File already saved\n");
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
    JsonDocument &app = status();

    // if no current file is specified then skip
    if (fileBuffer.getFileName().length() == 0)
    {
        //
        app["error"] = "Filename is empty. Can't clear.\n";
        app["screen"] = ERRORSCREEN;

        //
        _log(app["error"]);
        
        return;
    }

    // Step 1. Check if the backup file exists
    // remove it if already exists
    String backupFileName = format("/%s_backup.txt", fileBuffer.getFileName());

    if (gfs()->exists(backupFileName.c_str()))
    {
        // remove the backup file
        gfs()->remove(backupFileName.c_str());
    }

    // Step 2. Rename the current file to the backup.txt
    if (gfs()->rename(fileBuffer.getFileName().c_str(), backupFileName.c_str()))
    {
        _log("File renamed successfully: %s.\n", backupFileName);
    }
    else
    {
        app["error"] = format("Error making a backup file. %s\n", backupFileName);
        _log(app["error"].as<const char *>());
        app["screen"] = ERRORSCREEN;

        return;
    }

    // Step 3. Empty the current file by opening it with FILE_WRITE
    File file = gfs()->open(fileBuffer.getFileName().c_str(), "w");
    if (!file)
    {
        //
        app["error"] = format("Failed to create an empty file %s\n", fileBuffer.getFileName());
        app["screen"] = ERRORSCREEN;

        //
        _log(app["error"]);
        
        return;
    }

    // clean up file
    file.close();
    delay(100);

    // Step 4. Go through the loading process of the empty file
    loadFile(fileBuffer.getFileName());
}


// Handle Keyboard Input
void Editor::keyboard(char key)
{
    _debug("Editor::keyboard:: %c\n", key);

    //////////////////////////
    // BACKWARD EDITING
    //////////////////////////
    if (key == '\b')
    {
        // buffer has more than 1 character
        if (fileBuffer.getBufferSize() > 0)
        {
            fileBuffer.removeLastChar();
            // set saved flag to false
            this->saved = false;
        }
        // buffer emptied
        else
        {
            // Load previous contents from the file if at the beginning of the buffer
            _log("Backspace reached the beginning of the buffer\n");

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
            // if editing at the end of the line then remove word
            if (fileBuffer.cursorPos == fileBuffer.bufferSize)
            {
                fileBuffer.removeLastWord();
            }

            else
            {
                // remove word in front
                fileBuffer.removeCharAtCursor();
            }

            // set saved flag to false
            this->saved = false;
        }
        // buffer emptied
        else
        {
            // Load previous contents from the file if at the beginning of the buffer
            _log("Delete word reached the beginning of the buffer\n");

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
        if (key == 18)
        {
            if (fileBuffer.cursorPos == 0)
            {
                // load previous page
            }
            else
            {
                // left
                --fileBuffer.cursorPos;
            }
        }
        else if (key == 19)
        {
            // cursor can't move outside the last text
            if (fileBuffer.cursorPos < fileBuffer.getBufferSize())
                ++fileBuffer.cursorPos;
        }

        // UP
        else if (key == 20)
        {
            // move the cursorPos to the start of the previous line
            if (screenBuffer.cursorLine > 0)
            {
                // look at the previous line and move to the start of the cursor
                int newCursorPos =
                    screenBuffer.line_position[screenBuffer.cursorLine - 1] - screenBuffer.line_position[0];

                // cusor position in the line
                int cursorLinePos = screenBuffer.cursorLinePos;

                // if previous line length is shorter than cursorLinePos
                int previousLineLength = screenBuffer.line_length[screenBuffer.cursorLine - 1];
                if (previousLineLength < cursorLinePos)
                {
                    // then move to the end of the previous line
                    newCursorPos += previousLineLength - 1;
                }
                else
                {
                    // if previous line length is long enough then move as much as the currentLineCursorPos
                    newCursorPos += cursorLinePos;
                }

                // edge case
                if (newCursorPos < 0)
                    newCursorPos = 0;

                //
                fileBuffer.cursorPos = newCursorPos;
            }
        }

        // DOWN
        else if (key == 21)
        {
            // move the cursorPos to the start of the next line
            if (screenBuffer.cursorLine < screenBuffer.total_line)
            {
                //
                int newCursorPos =
                    screenBuffer.line_position[screenBuffer.cursorLine + 1] - screenBuffer.line_position[0];

                // cusor position in the line
                int cursorLinePos = screenBuffer.cursorLinePos;

                // next line length
                int nextLineLength = max(screenBuffer.line_length[screenBuffer.cursorLine + 1], 1);

                // if next line has shorter length of the current cursor pos
                // then move to the end of the next line
                if (cursorLinePos > nextLineLength)
                {
                    newCursorPos += nextLineLength - 1;
                }
                else
                {
                    //
                    newCursorPos += cursorLinePos;
                }

                // handle edge case
                if (newCursorPos > fileBuffer.bufferSize)
                    newCursorPos = fileBuffer.bufferSize;

                //
                fileBuffer.cursorPos = newCursorPos;
                screenBuffer.cursorLine += 1;
            }

            // when trying to go down at the last line, move the cursor to the end
            else if (screenBuffer.cursorLine == screenBuffer.total_line)
            {

                // if last line then move to the end of the buffer
                fileBuffer.cursorPos = fileBuffer.getBufferSize();

                _debug("Editor::keyboard::DOWN last line condition met cursorPos %d\n",
                          fileBuffer.cursorPos);
            }
        }

        // HOME
        else if (key == 2)
        {
            // home - move to the start of the line
            int newCursorPos =
                screenBuffer.line_position[screenBuffer.cursorLine] - screenBuffer.line_position[0];
            //
            fileBuffer.cursorPos = newCursorPos;
        }

        // END
        else if (key == 3)
        {
            // end - move to the end of the line
            int line_length = max(screenBuffer.line_length[screenBuffer.cursorLine], 1);
            int newCursorPos =
                screenBuffer.line_position[screenBuffer.cursorLine] - screenBuffer.line_position[0] + line_length - 1;

            // if last line then move to the end of the buffer
            if (screenBuffer.cursorLine == screenBuffer.total_line)
                newCursorPos = fileBuffer.getBufferSize();

            //
            fileBuffer.cursorPos = newCursorPos;
        }

        // PAGE UP
        else if (key == 22)
        {
            int newCursorLine = max(screenBuffer.cursorLine - screenBuffer.rows, 0);
            int newCursorPos =
                screenBuffer.line_position[newCursorLine] - screenBuffer.line_position[0];

            //
            fileBuffer.cursorPos = newCursorPos;
        }

        // PAGE DOWN
        else if (key == 23)
        {
            int newCursorLine = min(screenBuffer.cursorLine + screenBuffer.rows, screenBuffer.total_line);
            int line_length = max(screenBuffer.line_length[newCursorLine], 1);
            int newCursorPos =
                screenBuffer.line_position[newCursorLine] - screenBuffer.line_position[0] + line_length - 1;

            // if last line then move to the end of the buffer
            if (screenBuffer.cursorLine == screenBuffer.total_line)
                newCursorPos = fileBuffer.getBufferSize();

            //
            fileBuffer.cursorPos = newCursorPos;
        }
    }

    //////////////////////////
    // FORWARD EDITING
    //////////////////////////
    else
    {
        // add to the edit buffer new character
        if (!fileBuffer.available())
        {
            _log("Text buffer full\n");

            //
            saveFile();

            //
            loadFile(fileBuffer.fileName);
        }

        //
        fileBuffer.addChar(key);
        // set saved flag to false
        this->saved = false;
    }

    // update the screen buffer
    screenBuffer.Update(fileBuffer);
}
