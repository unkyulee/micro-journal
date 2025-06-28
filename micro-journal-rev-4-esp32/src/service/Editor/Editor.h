#ifndef Editor_h
#define Editor_h

//
#include <Arduino.h>

//
#include "FileBuffer/FileBuffer.h"

//
#include "ScreenBuffer/ScreenBuffer.h"

//
class Editor
{
public:
    // Initialize the editor with the number of columns and rows
    void init(int columns, int rows);

    // Editor Operation
    void loadFile(String fileName);
    void saveFile();
    void clearFile();

    // Handle Keyboard Inputs
    void keyboard(char key);

    // Saved Status
    bool saved = true;

    //
    FileBuffer fileBuffer;

    //
    ScreenBuffer screenBuffer;

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

#endif