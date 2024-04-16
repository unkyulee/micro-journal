#ifndef WORDPROCESSOR_h
#define WORDPROCESSOR_h

#include <TFT_eSPI.h>
#define FILENAME "/ujournal.txt"

class WordProcessor
{
public:
    // Static method to get the instance of the WordProcessor
    static WordProcessor& getInstance(TFT_eSPI *ptft);

    // Public methods
    void setup();
    void render();
    void keyboard(char key);
    void emptyFile();

private:
    // Private constructor to prevent instantiation
    WordProcessor(TFT_eSPI *ptft) : ptft(ptft) {}

    // Static instance of WordProcessor
    static WordProcessor* instance;

    // Constants
    // buffer size should be divisible by row character count (29)
    //static const int TEXT_BUFFER_SIZE = 2900;
    static const int TEXT_BUFFER_SIZE = 58;
    static const int MAX_ROW_CHARACTERS = 29;
    static const int MAX_LINES = 10;
    static const int STATUSBAR_Y = 224;
    static const int STATUSBAR_COLOR = TFT_BROWN;

    // Member variables
    TFT_eSPI *ptft;
    char text_buffer[TEXT_BUFFER_SIZE + 2];
    int text_pos = 0;
    int text_pos_prev = 0;
    size_t text_last_save_pos = 0;
    char *line_position[TEXT_BUFFER_SIZE / MAX_ROW_CHARACTERS];
    bool blink = false;
    int total_line_prev = 0;
    int start_line_prev = 0;
    bool clear = true;
    size_t fileSize = 0;

    // Private methods
    void loadText();
    void saveText();
    
    void clearBackground();
    void clearTrails();
    void blinkCarrot();
    void checkSaved();
    String formatNumberWithCommas(long num);
};

#endif