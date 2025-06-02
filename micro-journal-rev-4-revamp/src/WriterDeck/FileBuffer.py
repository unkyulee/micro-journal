BUFFER_SIZE = 4096  # or whatever size you need

class FileBuffer:
    def __init__(self):
        self.buffer = ""
        self.buffer_size = 0
        self.cursor_pos = 0
        self.file_name = ""
        self.file_size = 0

    def load(self, file_name):
        self.file_name = file_name
        try:
            with open(file_name, "r", encoding="utf-8") as f:
                self.buffer = f.read(BUFFER_SIZE)
                self.buffer_size = len(self.buffer)
                self.cursor_pos = self.buffer_size
                self.file_size = self.buffer_size
        except Exception as e:
            print(f"Load file failed: {e}")
            self.buffer = ""
            self.buffer_size = 0
            self.cursor_pos = 0
            self.file_size = 0

    def save(self):
        try:
            with open(self.file_name, "w", encoding="utf-8") as f:
                f.write(self.buffer)
                self.file_size = len(self.buffer)
        except Exception as e:
            print(f"Save file failed: {e}")

    def reset(self):
        self.buffer = ""
        self.buffer_size = 0
        self.cursor_pos = 0

    def get_buffer(self):
        return self.buffer

    def get_buffer_size(self):
        return self.buffer_size

    def add_char(self, c):
        if self.buffer_size < BUFFER_SIZE:
            self.buffer = (
                self.buffer[:self.cursor_pos] + c + self.buffer[self.cursor_pos:]
            )
            self.cursor_pos += 1
            self.buffer_size += 1

    def remove_last_char(self):
        if self.buffer_size > 0 and self.cursor_pos > 0:
            self.buffer = (
                self.buffer[: self.cursor_pos - 1] + self.buffer[self.cursor_pos :]
            )
            self.cursor_pos -= 1
            self.buffer_size -= 1

    def remove_char_at_cursor(self):
        if self.buffer_size > 0 and self.cursor_pos < self.buffer_size:
            self.buffer = (
                self.buffer[: self.cursor_pos] + self.buffer[self.cursor_pos + 1 :]
            )
            self.buffer_size -= 1

    def remove_last_word(self):
        if self.buffer_size == 0:
            return
        end = self.cursor_pos - 1
        while end >= 0 and self.buffer[end] == " ":
            end -= 1
        if end < 0:
            return
        start = end
        while start >= 0 and self.buffer[start] not in (" ", "\n"):
            start -= 1
        if start <= 0:
            self.buffer = ""
            self.buffer_size = 0
            self.cursor_pos = 0
        else:
            self.buffer = self.buffer[: start + 1]
            self.buffer_size = len(self.buffer)
            self.cursor_pos = self.buffer_size

    def available(self):
        return self.buffer_size < BUFFER_SIZE