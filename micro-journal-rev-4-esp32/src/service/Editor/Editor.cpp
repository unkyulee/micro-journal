#include "editor.h"
#include "app/app.h"
#include "display/display.h"

//
// EDITOR CLASS IMPLEMENTATION
//

//
// Editor Initialization with column and row setup
void Editor::init(int cols, int rows)
{
    // Define Screen Size
    this->cols = cols;
    this->rows = rows;

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

    // Save filen name
    this->fileName = fileName;

    // Open File
    File file = gfs()->open(fileName.c_str(), "r");
    if (!file)
    {
        //
        app["error"] = format("file open failed %s\n", fileName.c_str());
        app["screen"] = ERRORSCREEN;

        //
        _log(app["error"]);

        return;
    }

    // Determine file size and set buffer accordingly
    fileSize = file.size();
    _log("File: %s of size: %d\n", fileName.c_str(), fileSize);

    // calcualte the file offset
    seekPos = 0;
    int stepSize = BUFFER_SIZE / 2; // use half of the buffer
    if (fileSize > 0)
    {
        // this offset will offer last portion of the buffer
        seekPos = (fileSize / stepSize) * stepSize;
    }

    // when it is exactly the buffer end
    // go one buffer behind so that screen will show something
    if (fileSize == seekPos && seekPos > 0)
    {
        if (seekPos > stepSize)
            seekPos -= stepSize;
        else
            // defensive code in order for the offset not to go negative (MAX in unsigned in)
            seekPos = 0;
    }

    // move the file position to offset
    if (!file.seek(seekPos))
    {
        //
        file.close();
        delay(100);

        //
        app["error"] = format("Failed to seek file pointer. fileSize: %d seekPos: %d\n", fileSize, seekPos);
        app["screen"] = ERRORSCREEN;
        _log(app["error"].as<const char *>());

        return;
    }

    // reset the buffer
    resetBuffer();

    // Read file content into text buffer    
    int bufferSize = 0;
    while (file.available())
    {
        buffer[bufferSize++] = file.read();
    }
    cursorPos = bufferSize;

    //
    file.close();
    delay(100);

    // log
    _debug("Editor::loadFile size: %d, seek: %d, buffer: %d, cursor: %d\n", 
        fileSize, seekPos, bufferSize, cursorPos);

    // Update the Screen Buffer
    updateScreen();
}

//
void Editor::saveFile()
{
    //
    JsonDocument &app = status();

    // if no current file is specified then skip
    if (fileName.length() == 0)
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
        _log("File already saved. No operation required.\n");
        return;
    }

    //
    _log("Saving file %s\n", fileName);
    File file = gfs()->open(fileName.c_str(), "w");
    if (!file)
    {
        //
        app["error"] = "Failed to open file for writing\n";
        app["screen"] = ERRORSCREEN;

        //
        _log(app["error"]);

        return;
    }

    // Seek to the last loaded offset
    if (!file.seek(seekPos))
    {
        _log("Failed to seek file pointer\n");
        file.close();
        delay(100);

        return;
    }
    _log("Writing file at: %d\n", seekPos);

    // writing the file content
    size_t length = file.print(buffer);
    if (length >= 0)
    {
        _log("File written: %d bytes\n", length);
    }
    else
    {
        app["error"] = "Save failed\n";
        app["screen"] = ERRORSCREEN;
        _log(app["error"].as<const char *>());
    }

    //
    file.close();
    delay(100);

    // recalculate the file size
    // calculate the file size
    file = gfs()->open(fileName.c_str(), "r");
    if (!file)
    {
        //
        app["error"] = "Failed to open file for reading\n";
        app["screen"] = ERRORSCREEN;

        //
        _log(app["error"]);

        //
        return;
    }

    //
    fileSize = file.size();

    //
    file.close();
    delay(100);

    // flag to save
    this->saved = true;
}

// Make the current file empty
void Editor::clearFile()
{
    //
    JsonDocument &app = status();

    // if no current file is specified then skip
    if (fileName.length() == 0)
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
    String backupFileName = format("/%s_backup.txt", fileName.c_str());
    _debug("backupFilename: %s\n", backupFileName);
    if (gfs()->exists(backupFileName.c_str()))
    {
        // remove the backup file
        gfs()->remove(backupFileName.c_str());
    }

    // Step 2. Rename the current file to the backup.txt
    if (gfs()->rename(fileName.c_str(), backupFileName.c_str()))
    {
        _log("File renamed successfully: %s -> %s.\n", fileName.c_str(), backupFileName.c_str());
    }
    else
    {
        //
        app["error"] = format("Error making a backup file. %s\n", backupFileName);
        app["screen"] = ERRORSCREEN;    

        //
        _log(app["error"]);        

        return;
    }

    // Step 3. Empty the current file by opening it with FILE_WRITE
    File file = gfs()->open(fileName.c_str(), "w");
    if (!file)
    {
        //
        app["error"] = format("Failed to create an empty file %s\n", fileName.c_str());
        app["screen"] = ERRORSCREEN;

        //
        _log(app["error"]);

        return;
    }

    // clean up file
    file.close();
    delay(100);

    // Go through the loading process of the empty file
    loadFile(fileName);
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
        if (getBufferSize() > 0)
        {
            removeLastChar();
            // set saved flag to false
            this->saved = false;
        }
        // buffer emptied
        else
        {
            // Load previous contents from the file if at the beginning of the buffer
            _log("Backspace reached the beginning of the buffer\n");
        }
    }

    // DEL key deletes the word
    else if (key == 127)
    {
        if (getBufferSize() > 0)
        {
            // if editing at the end of the line then remove word
            if (cursorPos == getBufferSize())
            {
                removeLastWord();
            }

            else
            {
                // remove word in front
                removeCharAtCursor();
            }

            // set saved flag to false
            this->saved = false;
        }
        // buffer emptied
        else
        {
            // Load previous contents from the file if at the beginning of the buffer
            _log("Delete word reached the beginning of the buffer\n");
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
            if (cursorPos == 0)
            {
                // load previous page
            }
            else
            {
                // left
                --cursorPos;
            }
        }
        else if (key == 19)
        {
            // cursor can't move outside the last text
            if (cursorPos < getBufferSize())
                ++cursorPos;
        }

        // UP
        else if (key == 20)
        {
            // move the cursorPos to the start of the previous line
            if (cursorLine > 0)
            {
                // look at the previous line and move to the start of the cursor
                int newCursorPos =
                    linePositions[cursorLine - 1] - linePositions[0];

                // if previous line length is shorter than cursorLinePos
                int previousLineLength = lineLengths[cursorLine - 1];
                if (previousLineLength < cursorLinePos)
                {
                    // then move to the end of the previous line
                    newCursorPos += previousLineLength - 1;
                }
                else
                {
                    // if previous line length is long enough 
                    // then move as much as the currentLineCursorPos
                    newCursorPos += cursorLinePos;
                }

                // edge case
                if (newCursorPos < 0)
                    newCursorPos = 0;

                //
                cursorPos = newCursorPos;
            }
        }

        // DOWN
        else if (key == 21)
        {
            // move the cursorPos to the start of the next line
            if (cursorLine < totalLine)
            {
                //
                int newCursorPos =
                    linePositions[cursorLine + 1] - linePositions[0];

                // next line length
                int nextLineLength = max(lineLengths[cursorLine + 1], 1);

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
                if (newCursorPos > getBufferSize())
                    newCursorPos = getBufferSize();

                //
                cursorPos = newCursorPos;
                cursorLine += 1;
            }

            // when trying to go down at the last line, move the cursor to the end
            else if (cursorLine == totalLine)
            {

                // if last line then move to the end of the buffer
                cursorPos = getBufferSize();

                _debug("Editor::keyboard::DOWN last line condition met cursorPos %d\n",
                       cursorPos);
            }
        }

        // HOME
        else if (key == 2)
        {
            // home - move to the start of the line
            int newCursorPos =
                linePositions[cursorLine] - linePositions[0];
            //
            cursorPos = newCursorPos;
        }

        // END
        else if (key == 3)
        {
            // end - move to the end of the line
            int lineLength = max(lineLengths[cursorLine], 1);
            int newCursorPos =
                linePositions[cursorLine] - linePositions[0] + lineLength - 1;

            // if last line then move to the end of the buffer
            if (cursorLine == totalLine)
                newCursorPos = getBufferSize();

            //
            cursorPos = newCursorPos;
        }

        // PAGE UP
        else if (key == 22)
        {
            int newCursorLine = max(cursorLine - rows, 0);
            int newCursorPos =
                linePositions[newCursorLine] - linePositions[0];

            //
            cursorPos = newCursorPos;
        }

        // PAGE DOWN
        else if (key == 23)
        {
            int newCursorLine = min(cursorLine + rows, totalLine);
            int lineLength = max(lineLengths[newCursorLine], 1);
            int newCursorPos =
                linePositions[newCursorLine] - linePositions[0] + lineLength - 1;

            // if last line then move to the end of the buffer
            if (cursorLine == totalLine)
                newCursorPos = getBufferSize();

            //
            cursorPos = newCursorPos;
        }
    }

    //////////////////////////
    // FORWARD EDITING
    //////////////////////////
    else
    {
        // add to the edit buffer new character
        if (getBufferSize() > BUFFER_SIZE)
        {
            _log("Text buffer full\n");

            //
            saveFile();

            //
            loadFile(fileName);
        }

        //
        addChar(key);

        // set saved flag to false
        this->saved = false;
    }

    // update the screen buffer
    updateScreen();
}

//
void Editor::updateScreen()
{
    // Loop through the text buffer
    // and product the data structure that is splitted in each line    

    _debug("Editor::updateScreen called");

    // Handle empty buffer
    if (buffer[0] == '\0')
    {
        totalLine = 0;
        linePositions[0] = buffer;
        lineLengths[0] = 0;
        cursorLine = 0;
        cursorLinePos = 0;
        return;
    }

    // first line is the first of the buffer
    linePositions[0] = &buffer[0];
    lineLengths[0] = 0;

    //
    this->totalLine = 0;
    int line_count = 0;

    // remember the last space position to use it for the word wrap
    int last_space_index = -1;
    int last_space_position = -1;

    //
    // BUFFER -> SPLIT IN LINES
    //
    for (int i = 0; i < BUFFER_SIZE; i++) // Fixed loop condition
    {
        // When reaching the end of text, break
        if (buffer[i] == '\0')
        {
            // Update the length of the last line
            lineLengths[totalLine] = line_count;

            //
            break;
        }

        // Count total characters in a line
        line_count++;

        // Track the position of the last space
        if (buffer[i] == ' ')
        {
            last_space_index = i;
            last_space_position = line_count;
        } 
        
        // Handle words longer than `cols`
        if (line_count == cols && last_space_index == -1)
        {
            // Register the line count
            lineLengths[totalLine] = line_count;

            // Start a new line
            linePositions[++totalLine] = &buffer[i + 1];

            // Reset counters
            line_count = 0;
            last_space_index = -1;
            last_space_position = -1;

            continue;
        }
        // When receiving a newline or max characters reached, start a new line
        if (buffer[i] == '\n' || line_count == cols)
        {
            // when ENTER key is found
            if (buffer[i] == '\n')
            {
                // register the line count
                lineLengths[totalLine] = line_count;

                // start of the new line
                linePositions[++totalLine] = &buffer[i + 1];

                // reset counters
                line_count = 0;
            }

            // This line requires word-wrap
            else if (last_space_index != -1 && buffer[i] != '\n')
            {
                // register the line position as the last space position
                lineLengths[totalLine] = last_space_position;

                // new line starts from just after the space
                linePositions[++totalLine] = &buffer[last_space_index + 1];

                // new line count starts from the wrapped word
                line_count -= last_space_position;
            }

            // This line doesn't requrie word wrap
            else
            {
                // register the line count
                lineLengths[totalLine] = line_count;

                //
                linePositions[++totalLine] = &buffer[i + 1];

                //
                line_count = 0;
            }

            // reset the word wrap flags
            last_space_index = -1;
            last_space_position = -1;
        }
    }

    // Handle cursor position beyond buffer
    if (cursorPos >= BUFFER_SIZE)
    {
        cursorPos = BUFFER_SIZE - 1;
    }

    //
    // CALCULATE CURSOR INFORMATION
    //
    char *pCursorPos = &buffer[cursorPos];

    //
    cursorLine = 0;
    cursorLinePos = lineLengths[0];

    // caculate the which line cursor is located and the line position
    for (int i = totalLine; i >= 0; i--)
    {
        //
        if (pCursorPos >= linePositions[i])
        {
            // found the line index
            cursorLine = i;
            // calculate the cursor position within the line
            cursorLinePos = pCursorPos - linePositions[i];
            break;
        }
    }
}

void Editor::addChar(char c)
{
    int bufferSize = getBufferSize();
    if (bufferSize < BUFFER_SIZE)
    {
        // shift the trailing texts
        if (bufferSize > cursorPos)
            memmove(buffer + cursorPos + 1, buffer + cursorPos, bufferSize - cursorPos);

        //
        buffer[cursorPos++] = c;
        buffer[++bufferSize] = '\0';

        _debug("FileBuffer::addChar::cursorPos %d %c\n", cursorPos, c);
    }
}

void Editor::removeLastChar()
{
    int bufferSize = getBufferSize();
    if (bufferSize > 0 && cursorPos > 0)
    {
        // Shift the trailing texts left by one position
        if (bufferSize > cursorPos)
        {
            memmove(buffer + cursorPos - 1, buffer + cursorPos, bufferSize - cursorPos);
        }

        // Decrease buffer size and cursor position
        --bufferSize;
        --cursorPos;

        //
        _debug("FileBuffer::removeLastChar %d\n", cursorPos);

        // Null terminate the buffer
        buffer[bufferSize] = '\0';
    }
}

void Editor::removeCharAtCursor()
{
    int bufferSize = getBufferSize();
    if (bufferSize > 0 && cursorPos < bufferSize)
    {
        // Shift the trailing text left by one position
        if (bufferSize > cursorPos + 1)
        {
            memmove(buffer + cursorPos, buffer + cursorPos + 1, bufferSize - cursorPos - 1);
        }

        // Decrease buffer size
        --bufferSize;

        // Null terminate the buffer
        buffer[bufferSize] = '\0';
    }
}

void Editor::removeLastWord()
{
    int length = getBufferSize();
    if (length == 0)
        return;

    int end = length - 1;
    while (end >= 0 && buffer[end] == ' ')
        end--;

    if (end < 0)
        return;

    int start = end;
    while (start >= 0 && buffer[start] != ' ' && buffer[start] != '\n')
        start--;

    if (start <= 0)
    {
        start = 0;
        buffer[0] = '\0';        
    }
    else
    {
        buffer[start] = ' ';
        buffer[start + 1] = '\0';        
    }

    cursorPos = getBufferSize();

    //
    _debug("FileBuffer::removeLastWord %d\n", cursorPos);
}