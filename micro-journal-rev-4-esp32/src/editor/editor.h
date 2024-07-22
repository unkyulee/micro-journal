#ifndef Editor_h
#define Editor_h

#include <cstring> // For memset
#include <Arduino.h>

#define BUFFER_SIZE 1000

class Editor
{
public:
    // Static method to get the instance of the Editor
    static Editor &getInstance(int rows = 10, int cols = 26)
    {
        static Editor instance(rows, cols);
        return instance;
    }

    // Delete copy constructor and assignment operator to ensure singleton properties
    Editor(const Editor &) = delete;
    Editor &operator=(const Editor &) = delete;

    // Keyboard Input Handling
    void keyboard(char key);
    void delete_word();

    //
    void loadFile(String filename);
    void saveFile();
    void clearFile();

    // file size formatted
    String getFileSize();

    // lines
    char *line_position[BUFFER_SIZE + 2];
    int line_length[BUFFER_SIZE + 2];
    int total_line = 0;
    void calculate_screen_buffer();

    // Allowed Rows and Columns
    int rows;
    int cols;

    //
    int text_pos = 0;
    bool saved = false;
    bool saving = false;
    bool clear = false;

    // current filename
    String fileName;
    size_t fileSize;

private:
    // Private constructor to prevent instantiation
    Editor(int rows, int cols) : rows(rows), cols(cols)
    {
        //
        memset(buffer, 0, sizeof(buffer));
    }

    // Static instance of Editor
    static Editor *instance;

    // Buffer size and buffer array
    char buffer[BUFFER_SIZE + 24]; // Extra space for overflow handling
};

#endif