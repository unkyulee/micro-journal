//
#ifndef FileBuffer_h
#define FileBuffer_h

//
#include <Arduino.h>
#include <cstring> // For memset

#define BUFFER_SIZE 4000

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


    //
    void reset();

    //
    char *getBuffer();

    //
    int getBufferSize();
    void addChar(char c);
    void removeLastChar();
    void removeCharAtCursor();
    void removeLastWord();

    //
    bool available();
};

#endif