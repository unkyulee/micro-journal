#pragma once

//
#include <Arduino.h>

//
#include "EditorFont.h"
#include "HangulComposer.h"
#include "Utf8.h"

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

    // How many bytes of the current window came from disk at last load/save.
    // Together with seekPos this defines the on-disk region [seekPos, seekPos+loadedLength)
    // that the buffer mirrors, independent of in-memory edits that may have
    // grown or shrunk the buffer since.
    size_t loadedLength = 0;

    // Set whenever the buffer is swapped to a different window of the file
    // (paging, or the buffer filling up while typing) so display code knows
    // to do a full redraw instead of an incremental one.
    bool pageChanged = false;

    // Set whenever the character before the cursor was replaced in place
    // (Hangul composition: ㄱ -> 가 -> 간). Displays that only repaint
    // appended text must do a full repaint of the line, because the new
    // glyph doesn't cover the old one's pixels. Display code consumes it.
    bool charReplaced = false;

    // Screen Size Definition
    // cols is measured in display columns, not bytes: a single-width glyph
    // occupies 1 column, a double-width glyph (e.g. Hangul) occupies 2
    int rows = 10;
    int cols = 26;

    // Active font. Owned by the display backend that registered it;
    // the editor only reads metrics and the charColumns hook from it.
    const EditorFont *font = nullptr;

    // Display columns occupied by one codepoint with the active font.
    // This is the single point where double-width (Korean) support will
    // plug in - everything that measures text goes through here.
    uint8_t charColumns(uint32_t codepoint)
    {
        if (font && font->charColumns)
            return font->charColumns(codepoint);
        return 1;
    }

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

    // Cursor position within the line, in bytes
    int cursorLinePos = 0;

    // Cursor position within the line, in display columns.
    // Displays must use this (not cursorLinePos) for cursor x placement so
    // the math stays correct once double-width glyphs exist.
    int cursorLineCols = 0;

    // Word Counter File
    int wordCountFile = 0;
    int wordCountBuffer = 0;

    // Initialize the editor with the number of columns and rows.
    // font is optional so display backends that still use a fixed layout
    // keep working; when provided, its charColumns hook drives line wrap.
    void init(int cols, int rows, const EditorFont *font = nullptr);
    void loop(); // house keeping tasks

    // File Operation
    void loadFile(String fileName);
    bool saveFile();
    void clearFile();

    // Paging: slide the in-memory window to a different part of the file
    bool loadWindow(size_t offset, size_t length);
    void pageBackward();
    void pageForward();
    void advanceWindow();

    //
    bool savingInProgress = false;

    // Handle Keyboard Inputs
    void keyboard(int key, bool pressed);
    int lastKey = 0; // int: keys above 127 must not be narrowed
    unsigned long lastPressTime = 0;
    unsigned long repeatInterval = 80; // ms between repeats
    unsigned long repeatDelay = 300;   // ms before repeat starts
    bool backSpacePressed = false;

    //
    int getBufferSize() { return strlen(buffer); }
    void resetBuffer() { memset(buffer, '\0', sizeof(buffer)); }

    // The buffer holds UTF-8: addChar takes a codepoint and encodes it,
    // the remove operations always delete whole characters.
    void addChar(int c);
    void removeLastChar();
    void removeCharAtCursor();
    void removeLastWord();

    // Hangul composition: jamo keystrokes are assembled into syllables and
    // the character before the cursor is updated in place as it evolves
    HangulComposer composer;
    void replaceCharBeforeCursor(uint32_t codepoint);

    // UTF-8 character navigation over the buffer. Positions passed in and
    // returned are byte offsets that land on character boundaries.
    int snapToCharStart(int pos); // back up to the lead byte at/before pos
    int prevCharStart(int pos);   // start of the character before pos
    int nextCharStart(int pos);   // start of the character after pos

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
