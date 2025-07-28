#include "WordCounter.h"
#include "app/app.h"
#include "display/display.h"
#include "service/Editor/Editor.h"

// 28.7.2025
// Credit to one of the forks from "frankfurtsky"
// https://github.com/frankfurtsky/micro-journal

// counts word count until the last buffer
int wordcounter_file(const char *filename)
{
    int wordCount = 0;
    bool inWord = false;

    //
    JsonDocument &app = status();
    _log("Editor loading file %s\n", filename);

    // Open File
    File file = gfs()->open(filename, "r");
    if (!file)
    {
        //
        app["error"] = format("file open failed %s\n", filename);
        app["screen"] = ERRORSCREEN;

        //
        _log(app["error"]);

        return 0;
    }

    // Size of the data not going into the buffer
    uint32_t fileSize = file.size();
    int sizeOfDataNotInBuffer = fileSize - (BUFFER_SIZE / 2);

    // Move the pointer to start of the file
    int readLimit = (fileSize > (BUFFER_SIZE / 2)) ? sizeOfDataNotInBuffer : 0;

    // Move file pointer
    file.seek(0);

    const size_t CHUNK_SIZE = 1000;
    char buffer[CHUNK_SIZE];
    uint32_t bytesRead = 0;

    _log("read limit:  %d \n", readLimit);
    // Count the words until the part of the file that will go into the buffer
    while (file.available() && bytesRead < readLimit)
    {
        size_t toRead = std::min((size_t)(readLimit - bytesRead), CHUNK_SIZE);
        size_t readNow = file.readBytes(buffer, toRead);
        bytesRead += readNow;

        for (size_t i = 0; i < readNow; ++i)
        {
            char c = buffer[i];
            if (isAlphaNumeric(c))
            {
                if (!inWord)
                {
                    inWord = true;
                    wordCount++;
                }
            }
            else
            {
                inWord = false;
            }
        }
    }

    _log("Words in file:  %d \n", wordCount);
    return wordCount;
}

// counts word count inside the buffer
int wordcounter_buffer(const char *buffer)
{
    int count = 0;
    bool inWord = false;

    for (size_t i = 0; buffer[i] != '\0'; ++i)
    {
        if (BUFFER_SIZE <= i)
            break;

        if (std::isalnum(buffer[i]))
        {
            if (!inWord)
            {
                inWord = true;
                ++count;
            }
        }
        else
        {
            inWord = false;
        }
    }

    _log("Words in buffer:  %d \n", count);
    return count;
}