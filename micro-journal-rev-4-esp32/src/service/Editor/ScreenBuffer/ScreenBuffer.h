#include "service/Editor/Editor.h"

#ifndef ScreenBuffer_h
#define ScreenBuffer_h

#define SCREEN_BUFFER_SIZE 4000

//
class ScreenBuffer
{
public:
    //
    int rows = 10;
    int cols = 26;
    

    //
    int cursorLine = 0;
    int cursorLinePos = 0;

    //
    char *line_position[SCREEN_BUFFER_SIZE + 2];
    int line_length[SCREEN_BUFFER_SIZE + 2];
    int total_line = 0;

    bool clear_later = false;
    unsigned int clear_last = 0;

    //
    void Update(FileBuffer &fileBuffer);
};

#endif