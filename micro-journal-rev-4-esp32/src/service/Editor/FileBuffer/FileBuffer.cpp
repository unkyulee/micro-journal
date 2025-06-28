#include "FileBuffer.h"
#include "app/app.h"

// to identify the screen
#include "display/display.h"

//
int FileBuffer::getSeekPos()
{
    return seekPos;
}

//
String FileBuffer::getFileName()
{
    return fileName;
}

// Fill the buffer from File
void FileBuffer::load(String fileName)
{
    // app status
    JsonDocument &app = status();

    //
    if (fileName.length() == 0)
    {
        //
        app["error"] = "Load file failed. File name is empty.";
        app["screen"] = ERRORSCREEN;

        return;
    }

    // Save filen name
    this->fileName = fileName;

    // Check if the file exists, create if not
    if (!gfs()->exists(fileName.c_str()))
    {
        File file = gfs()->open(fileName.c_str(), "w");
        if (!file)
        {
            //
            file.close();
            delay(100);

            //
            app["error"] = "Failed to create file";
            app["screen"] = ERRORSCREEN;

            return;
        }

        //
        file.close();
        delay(100);
    }

    // open the text file
    _log("Loading file %s\n", fileName.c_str());
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
    reset();

    // Read file content into text buffer
    bufferSize = 0;
    buffer[bufferSize] = '\0';
    while (file.available())
    {
        buffer[bufferSize++] = file.read();
    }
    cursorPos = bufferSize;

    //
    file.close();
    delay(100);

    // log
    _debug("FileBuffer::load::Read size: %d, seek: %d, buffer: %d, cursor: %d\n", fileSize, seekPos, bufferSize, cursorPos);
}

void FileBuffer::save()
{
    // app status
    JsonDocument &app = status();

    //
    _log("Saving file %s\n", fileName);
    File file = gfs()->open(fileName.c_str(), "w");
    if (!file)
    {
        //
        app["error"] = "Failed to open file for writing\n";
        app["screen"] = ERRORSCREEN;
        _log(app["error"].as<const char *>());

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
}

//
void FileBuffer::reset()
{
    //
    memset(buffer, '\0', sizeof(buffer));

    //
    bufferSize = 0;
}

//
char *FileBuffer::getBuffer()
{
    return buffer;
}

//
int FileBuffer::getBufferSize()
{
    return bufferSize;
}

void FileBuffer::addChar(char c)
{
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

void FileBuffer::removeLastChar()
{
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

void FileBuffer::removeCharAtCursor()
{
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

void FileBuffer::removeLastWord()
{
    int length = bufferSize;
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
        bufferSize = 0;
    }
    else
    {
        buffer[start] = ' ';
        buffer[start + 1] = '\0';
        bufferSize = start + 1;
    }

    cursorPos = bufferSize;

    //
    _debug("FileBuffer::removeLastWord %d\n", cursorPos);
}

//
bool FileBuffer::available()
{
    // is there still buffer?
    return bufferSize < BUFFER_SIZE;
}