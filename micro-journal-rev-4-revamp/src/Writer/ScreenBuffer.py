# Screen buffer will split the lines of the file buffer into multiple lines
# to fit the screen size. It will calculate the word wraps and line breaks 
# in order to split the line logically based on the screen width.
class ScreenBuffer:
    def __init__(self, cols=10, max_lines=100):
        # Configuration
        self.cols = cols
        self.max_lines = max_lines
        
        #
        # Data Structure
        #
        # Line position will store the starting index of each line position in the buffer
        self.line_position = [0] * max_lines
        
        # Each line length will be stored 
        self.line_length = [0] * max_lines
        
        # Total number of lines in the buffer
        self.total_line = 0

    # Every changes in the buffer will trigger an update of the screen buffer
    def update(self, file_buffer):
        
        #
        self.total_line = 0
            
        #
        self.line_position[0] = 0
        self.line_length[0] = 0
        
        # reset cursor line information
        # this is to be calculated  
        file_buffer.cursor_line = 0
        file_buffer.cursor_line_pos = 0
            
        # retrieve the buffer from the file buffer
        buffer = file_buffer.get_buffer()
        
        # If the buffer is empty, reset the screen buffer
        if not buffer or len(buffer) == 0:
            # return as there are no calculation needed
            return


        
        # Space tracking for word wrapping
        last_space_index = -1
        last_space_position = -1


        # Iterate through the buffer to calculate line breaks
        current_line = 0
        current_line_length = 0
        buffer_len = len(buffer)
        
        # Iterate through the buffer until we reach the end or max lines
        # or the end of the buffer
        for i, c in enumerate(buffer):
            # If we have reached the maximum lines, break the loop
            if self.total_line >= self.max_lines:
                break   
            
            # Get the current character
            c = buffer[i]
            
            # increase the current line count
            current_line_length += 1
         
            # If space is found then remmember the position
            # for the next word wrap
            if c == " ":
                last_space_index = i
                last_space_position = current_line_length
                
            # When there are new line characters, we need to break the line
            if c == "\n":
                # Increment the total line count
                self.total_line += 1
                
                # Save the current line length
                # Do not count the newline character because we don't want that to be rendered in the screen
                self.line_length[self.total_line] = current_line_length -1
                                
                # Set the next line starting position
                self.line_position[self.total_line] = i + 1
                
                # Reset the current line length and start a new line           
                current_line_length = 0
                
                # Reset the last space index and position 
                # start counting new from the new line
                last_space_index = -1
                last_space_position = -1
                
                continue

            # When current line count exceeds the column limit
            # Then we need to break the line
            # last_space_index is checked to see if the line needs broken at the last space for word wrapping
            if current_line_length == self.cols and last_space_index == -1:
                # Increment the total line count
                self.total_line += 1
                
                # Save the current line length
                self.line_length[self.total_line] = current_line_length
                                
                # Set the next line starting position
                self.line_position[self.total_line] = i + 1
                
                # Reset the current line length and start a new line           
                current_line_length = 0
                
                # Reset the last space index and position 
                # start counting new from the new line
                last_space_index = -1
                last_space_position = -1
                
                continue
                        
            # Word Wrap Required
            if current_line_length == self.cols and last_space_index != -1:
                # Increment the total line count
                self.total_line += 1

                # Save the current line length at the last space position
                self.line_length[self.total_line] = last_space_position
                
                # Set the next line starting position at the last space index + 1
                self.line_position[self.total_line] = last_space_index + 1
                
                # Reset the current line length to the remaining characters after the last space        
                self.line_length[self.total_line] = last_space_position

                # New line starts with the character after the last space     
                current_line_length -= last_space_position
                
                # Reset the last space index and position 
                # start counting new from the new line
                last_space_index = -1
                last_space_position = -1
                
                continue
                
            # This line does not need a word wrap
            if current_line_length == self.cols:
                
                # Increment the total line count
                self.total_line += 1
                
                # Save the current line length
                self.line_length[self.total_line] = current_line_length
                                
                # Set the next line starting position
                self.line_position[self.total_line] = i + 1
                
                # Reset the current line length and start a new line           
                current_line_length = 0
                
                # Reset the last space index and position 
                # start counting new from the new line
                last_space_index = -1
                last_space_position = -1
                
                continue

       ## TBD
        # Cursor position can exceed buffer length if it was set before the buffer was updated
        if file_buffer.cursor_pos > buffer_len:
            file_buffer.cursor_pos = buffer_len 

        # If the cursor is at the end of the buffer, set it to the last line
        p_cursor_pos = file_buffer.cursor_pos
        file_buffer.cursor_line = 0
        file_buffer.cursor_line_pos = self.line_length[0]

        for idx in range(self.total_line, -1, -1):
            if p_cursor_pos >= self.line_position[idx]:
                file_buffer.cursor_line = idx
                file_buffer.cursor_line_pos = p_cursor_pos - self.line_position[idx]
                break
            
        # increment the current line position
        current_line += 1
            
            
        # debug
        print(f"ScreenBuffer updated: total_line={self.total_line}, cursor_line={file_buffer.cursor_line}, cursor_line_pos={file_buffer.cursor_line_pos}")
        print(self.line_position[:self.total_line + 1])     
        print(self.line_length[:self.total_line + 1])  