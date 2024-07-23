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
    char* buffer = fileBuffer.getBuffer();    
    line_position[0] = &buffer[0];
    line_length[0] = cols;

    //
    for (int i = 0; i < BUFFER_SIZE+1; i++)
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
                line_length[total_line - 1] = row_character_count - 1;

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
}