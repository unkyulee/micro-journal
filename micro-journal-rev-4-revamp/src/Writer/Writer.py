from Writer.FileBuffer import FileBuffer
from Writer.ScreenBuffer import ScreenBuffer

# TBD
class Writer:
    
    def setup(self):
        pass
        
        
    def loop(self):
        pass
    
    # Singleton pattern for Writer class
    _instance = None

    def __new__(cls, *args, **kwargs):
        if not cls._instance:
            cls._instance = super(Writer, cls).__new__(cls)
        return cls._instance

    def __init__(self):
        # Only initialize once
        if not hasattr(self, "initialized"):
            self.file_buffer = FileBuffer()
            
            #
            self.screen_buffer = ScreenBuffer(cols=4, max_lines=100)
            self.saved = True
            self.initialized = True

    def load_file(self, file_name):
        if not file_name:
            print("Filename is empty. Can't load.")
            return

        # Step 1: Create file if necessary
        try:
            with open(file_name, "a", encoding="utf-8"):
                pass
        except Exception as e:
            print(f"Failed to create a file. {file_name}: {e}")
            return

        # Step 2: Initialize the FileBuffer
        self.file_buffer.load(file_name)

        # Update the Screen Buffer
        self.screen_buffer.update(self.file_buffer)
        

    def save_file(self):
        if not self.file_buffer.file_name:
            print("Filename is empty. Can't save.")
            return

        if self.saved:
            print("File already saved")
            return

        self.file_buffer.save()
        self.saved = True

    def clear_file(self):
        file_name = self.file_buffer.file_name
        if not file_name:
            print("Filename is empty. Can't clear.")
            return

        # Step 1: Remove backup if exists
        backup_file_name = f"{file_name}_backup.txt"
        import os
        if os.path.exists(backup_file_name):
            os.remove(backup_file_name)

        # Step 2: Rename current file to backup
        try:
            os.rename(file_name, backup_file_name)
            print(f"File renamed successfully: {backup_file_name}")
        except Exception as e:
            print(f"Error making a backup file. {backup_file_name}: {e}")
            return

        # Step 3: Empty the current file
        try:
            with open(file_name, "w", encoding="utf-8"):
                pass
        except Exception as e:
            print(f"Failed to create an empty file {file_name}: {e}")
            return

        # Step 4: Reload the empty file
        self.load_file(file_name)

    def handle_key_event(self, pressed, key):
        # Only handle key press events
        if not pressed:
            return

        # BACKSPACE
        if key == '\b':
            if self.file_buffer.get_buffer_size() > 0:
                self.file_buffer.remove_last_char()
                self.saved = False
            else:
                print("Backspace reached the beginning of the buffer")
                self.save_file()
                self.load_file(self.file_buffer.file_name)

        # DELETE (DEL key, ASCII 127)
        elif key == chr(127):
            if self.file_buffer.get_buffer_size() > 0:
                if self.file_buffer.cursor_pos == self.file_buffer.buffer_size:
                    self.file_buffer.remove_last_word()
                else:
                    self.file_buffer.remove_char_at_cursor()
                self.saved = False
            else:
                print("Delete word reached the beginning of the buffer")
                self.save_file()
                self.load_file(self.file_buffer.file_name)

        # CURSOR KEYS (simulate with int codes or custom mapping)
        elif isinstance(key, int):
            # Left
            if key == 18:
                if self.file_buffer.cursor_pos > 0:
                    self.file_buffer.cursor_pos -= 1
            # Right
            elif key == 19:
                if self.file_buffer.cursor_pos < self.file_buffer.get_buffer_size():
                    self.file_buffer.cursor_pos += 1
            # Up
            elif key == 20:
                if self.file_buffer.cursor_line > 0:
                    new_cursor_pos = (
                        self.screen_buffer.line_position[self.file_buffer.cursor_line - 1]
                        - self.screen_buffer.line_position[0]
                    )
                    cursor_line_pos = self.file_buffer.cursor_line_pos
                    prev_line_length = self.screen_buffer.line_length[self.file_buffer.cursor_line - 1]
                    if prev_line_length < cursor_line_pos:
                        new_cursor_pos += prev_line_length - 1
                    else:
                        new_cursor_pos += cursor_line_pos
                    if new_cursor_pos < 0:
                        new_cursor_pos = 0
                    self.file_buffer.cursor_pos = new_cursor_pos
            # Down
            elif key == 21:
                if self.file_buffer.cursor_line < self.screen_buffer.total_line:
                    new_cursor_pos = (
                        self.screen_buffer.line_position[self.file_buffer.cursor_line + 1]
                        - self.screen_buffer.line_position[0]
                    )
                    cursor_line_pos = self.file_buffer.cursor_line_pos
                    next_line_length = max(self.screen_buffer.line_length[self.file_buffer.cursor_line + 1], 1)
                    if cursor_line_pos > next_line_length:
                        new_cursor_pos += next_line_length - 1
                    else:
                        new_cursor_pos += cursor_line_pos
                    if new_cursor_pos > self.file_buffer.buffer_size:
                        new_cursor_pos = self.file_buffer.buffer_size
                    self.file_buffer.cursor_pos = new_cursor_pos
                    self.file_buffer.cursor_line += 1
                elif self.file_buffer.cursor_line == self.screen_buffer.total_line:
                    self.file_buffer.cursor_pos = self.file_buffer.get_buffer_size()
            # Home
            elif key == 2:
                new_cursor_pos = (
                    self.screen_buffer.line_position[self.file_buffer.cursor_line]
                    - self.screen_buffer.line_position[0]
                )
                self.file_buffer.cursor_pos = new_cursor_pos
            # End
            elif key == 3:
                line_length = max(self.screen_buffer.line_length[self.file_buffer.cursor_line], 1)
                new_cursor_pos = (
                    self.screen_buffer.line_position[self.file_buffer.cursor_line]
                    - self.screen_buffer.line_position[0]
                    + line_length - 1
                )
                if self.file_buffer.cursor_line == self.screen_buffer.total_line:
                    new_cursor_pos = self.file_buffer.get_buffer_size()
                self.file_buffer.cursor_pos = new_cursor_pos
            # Page Up
            elif key == 22:
                new_cursor_line = max(self.file_buffer.cursor_line - self.screen_buffer.max_lines, 0)
                new_cursor_pos = (
                    self.screen_buffer.line_position[new_cursor_line]
                    - self.screen_buffer.line_position[0]
                )
                self.file_buffer.cursor_pos = new_cursor_pos
            # Page Down
            elif key == 23:
                new_cursor_line = min(
                    self.file_buffer.cursor_line + self.screen_buffer.max_lines,
                    self.screen_buffer.total_line,
                )
                line_length = max(self.screen_buffer.line_length[new_cursor_line], 1)
                new_cursor_pos = (
                    self.screen_buffer.line_position[new_cursor_line]
                    - self.screen_buffer.line_position[0]
                    + line_length - 1
                )
                if self.file_buffer.cursor_line == self.screen_buffer.total_line:
                    new_cursor_pos = self.file_buffer.get_buffer_size()
                self.file_buffer.cursor_pos = new_cursor_pos

        # FORWARD EDITING (add character)
        elif isinstance(key, str) and len(key) == 1:
            if not self.file_buffer.available():
                print("Text buffer full")
                self.save_file()
                self.load_file(self.file_buffer.file_name)
            self.file_buffer.add_char(key)
            self.saved = False

        # Update the screen buffer after any change
        self.screen_buffer.update(self.file_buffer)
        
        # debug
        # print(self.file_buffer.get_buffer())

# Global singleton instance
writer = Writer()