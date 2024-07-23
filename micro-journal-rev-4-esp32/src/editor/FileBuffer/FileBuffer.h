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
    //
    char buffer[BUFFER_SIZE + 2];
    int bufferSize = 0;

    //
    String getFileName();
    //
    int getSeekPos();
    //
    void load(String fileName, size_t cursorPos);
    void save();

    //
    String fileName;
    //
    size_t fileSize = 0;
    //
    size_t seekPos = 0;
    // 
    int cursorPos = 0;
    
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