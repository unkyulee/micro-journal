#include "ScreenBuffer.h"
#include "app/app.h"
#include "display/display.h"



//
void ScreenBuffer::Update(FileBuffer &fileBuffer)
{
    // Initialize the screen text lines
    this->total_line = 0;
    int row_character_count = 0;
    int last_space_index = -1;
    int last_space_position = -1;

    //
    char *buffer = fileBuffer.getBuffer();
    line_position[0] = &buffer[0];
    line_length[0] = cols;

    //
    for (int i = 0; i < BUFFER_SIZE + 1; i++)
    {
        // When reaching the end of text, break
        if (buffer[i] == '\0')
        {
            // Update the length of the last line
            line_length[total_line] = row_character_count;

            // Mark the end of the text
            line_position[total_line + 1] = nullptr;
            line_length[total_line + 1] = 0;

            //
            break;
        }

        // Count total characters in a line
        row_character_count++;

        // Track the position of the last space
        if (buffer[i] == ' ')
        {
            last_space_index = i;
            last_space_position = row_character_count;
        }

        // When receiving a newline or max characters reached, start a new line
        if (buffer[i] == '\n' || row_character_count == cols)
        {
            if (buffer[i] == '\n')
            {
                // Handle newline character
                total_line++;
                line_position[total_line] = &buffer[i + 1];

                // new line is not included in the line
                line_length[total_line - 1] = row_character_count;

                // reset counters
                row_character_count = 0;
                last_space_index = -1;
                last_space_position = -1;
            }
            else if (last_space_index != -1 && buffer[i] != '\n')
            {
                // If there's a space within the line, wrap at the last space
                int wrap_position = last_space_position;
                total_line++;
                line_position[total_line] = &buffer[last_space_index + 1];
                line_length[total_line - 1] = wrap_position;

                //
                row_character_count -= wrap_position;
                last_space_index = -1;
                last_space_position = -1;
            }
            else
            {
                // If no spaces to wrap at, wrap at the max character limit
                total_line++;
                line_position[total_line] = &buffer[i + 1];
                line_length[total_line - 1] = row_character_count;
                row_character_count = 0;
            }
        }
    }

    // find the line where cursor is located at
    char *pCursorPos = &fileBuffer.buffer[fileBuffer.cursorPos];
    fileBuffer.cursorLine = total_line;
    fileBuffer.cursorLinePos = pCursorPos - line_position[total_line];

    // caculate the which line cursor is located and the line position
    for (int i = 0; i <= total_line; i++)
    {
        if (pCursorPos < line_position[i])
        {
            fileBuffer.cursorLine = i - 1;
            fileBuffer.cursorLinePos = pCursorPos - line_position[i - 1];
            break;
        }
    }

    // decide the START LINE
    // which line to be displayed as a first line in the display

    // when total line is less than the screen size then the start line is 0
    static int start_line_prev = -1;
    if (total_line < rows)
    {
        start_line = 0;
    }
    else if (total_line > rows)
    {
        // there are cases when the screen will flow out and need to restart from the top
        // in that case flip the page, and make it total_line - 2
        if (fileBuffer.cursorLine - start_line >= rows)
        {
            start_line = fileBuffer.cursorLine - 2;
            if (start_line < 0)
                start_line = 0;
        }

        // there can be cases when cursor goes up or
        // due to back space
        // cursorline may go before the start_line
        // in this case, realign the print
        else if (fileBuffer.cursorLine < start_line)
        {
            start_line = fileBuffer.cursorLine - rows + 1;
            if (start_line < 0)
                start_line = 0;
        }
    }

    // when start line changes clear
    if(start_line != start_line_prev) {
        //
        clear = true;
        start_line_prev = start_line;
    }

    // when the edit is happening while the cursor is moving
    // do the delay clear
    static int cursorPos_prev = 0;
    if(fileBuffer.cursorPos != fileBuffer.getBufferSize() && cursorPos_prev != fileBuffer.cursorPos) {
        delayedClear(500);
        cursorPos_prev = fileBuffer.cursorPos;
    }
}

// initiate clear command after certain time
// in order reduce the flicker
void ScreenBuffer::delayedClear(int millseconds)
{
    clear_last = millis() + millseconds;
    clear_later = true;
}

void ScreenBuffer::loop() {
    if(clear_later && millis() > clear_last) {
        clear_later = false;        
        clear = true;
    }
}