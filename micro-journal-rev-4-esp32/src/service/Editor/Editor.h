#pragma once

//
#include <Arduino.h>

#ifdef BOARD_PICO
#define BUFFER_SIZE 2000
#endif

#ifdef BOARD_ESP32_S3
#define BUFFER_SIZE 8000
#endif

//
class Editor
{
public:
    // Text Buffer
    char buffer[BUFFER_SIZE + 100];

    // Saved Status
    bool saved = true;

    // Current File Name
    String fileName;

    // Current File Total Size
    size_t fileSize = 0;

    // Last File Load Position
    // This is a place where save should occurs with current buffer
    size_t seekPos = 0;

    // Screen Size Definition
    int rows = 10;
    int cols = 26;

    // Each line starting point is saved in this array
    char *linePositions[BUFFER_SIZE + 2];

    // Length of each line
    int lineLengths[BUFFER_SIZE + 2];

    // Total number of lines in the buffer
    int totalLine = 0;

    // Cursor Position in terms of buffer
    int cursorPos = 0;

    // Which line the cursor is placed
    int cursorLine = 0;

    // Cursor position within the line
    int cursorLinePos = 0;

    // Word Counter File
    int wordCountFile = 0;
    int wordCountBuffer = 0;

    // Initialize the editor with the number of columns and rows
    void init(int cols, int rows);
    void loop(); // house keeping tasks

    // File Operation
    void loadFile(String fileName);
    void saveFile();
    void clearFile();

    //
    bool savingInProgress = false;

    // Handle Keyboard Inputs
    void keyboard(int key, bool pressed);
    char lastKey = 0;
    unsigned long lastPressTime = 0;
    unsigned long repeatInterval = 80; // ms between repeats
    unsigned long repeatDelay = 300;   // ms before repeat starts
    bool backSpacePressed = false;

    //
    int getBufferSize() { return strlen(buffer); }
    void resetBuffer() { memset(buffer, '\0', sizeof(buffer)); }

    //
    void addChar(int c);
    void removeLastChar();
    void removeCharAtCursor();
    void removeLastWord();

    //
    void updateScreen();

    //////////////////////////////////
    // SINGLETON PATTERN
    // Static method to get the instance of the Editor
    static Editor &getInstance()
    {
        static Editor instance;
        return instance;
    }

    // Delete copy constructor and assignment operator to ensure singleton properties
    Editor(const Editor &) = delete;
    Editor &operator=(const Editor &) = delete;
    //////////////////////////////////

private:
    // Private constructor to prevent instantiation
    Editor() {}
};
