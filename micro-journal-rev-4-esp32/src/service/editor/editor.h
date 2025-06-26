#ifndef Editor_h
#define Editor_h

//
#include <Arduino.h>

//
#include "FileBuffer/FileBuffer.h"
#include "ScreenBuffer/ScreenBuffer.h"

//
class Editor
{
public:
    // Static method to get the instance of the Editor
    static Editor &getInstance()
    {
        static Editor instance;
        return instance;
    }

    // Delete copy constructor and assignment operator to ensure singleton properties
    Editor(const Editor &) = delete;
    Editor &operator=(const Editor &) = delete;

    //
    FileBuffer fileBuffer; 

    //
    ScreenBuffer screenBuffer;

    // Editor Operation
    void loadFile(String fileName);
    void saveFile();
    void clearFile();

    // Handle Keyboard Inputs
    void keyboard(char key);

    // Saved Status
    bool saved = true;

    // File Operation On-going
    bool locked = false;

private:
    // Private constructor to prevent instantiation
    Editor() {}
};

#endif