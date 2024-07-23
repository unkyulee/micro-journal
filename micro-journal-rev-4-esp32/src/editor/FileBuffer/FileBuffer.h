//
#ifndef FileBuffer_h
#define FileBuffer_h

//
#include <Arduino.h>
#include <cstring> // For memset

#define BUFFER_SIZE 1000

//
class FileBuffer
{
public:
    FileBuffer()
    {
        bufferSize = 0;
        fileSize = 0;
        seekPos = 0;
        cursorPos = 0;
        cursorLine = 0;
        cursorLinePos = 0;
    }

    //
    char buffer[BUFFER_SIZE + 2];
    int bufferSize;

    //
    String getFileName();
    //
    int getSeekPos();
    //
    void load(String fileName);
    void save();

    //
    String fileName;
    //
    size_t fileSize;
    //
    size_t seekPos;
    //
    int cursorPos;
    int cursorLine;
    int cursorLinePos;

    //
    void reset();

    //
    char *getBuffer();

    //
    int getBufferSize();
    void addChar(char c);
    void removeLastChar();
    void removeLastWord();

    //
    bool available();
};

#endif