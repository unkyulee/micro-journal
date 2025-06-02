class ScreenBuffer:
    def __init__(self, cols=40, max_lines=100):
        self.cols = cols
        self.max_lines = max_lines
        self.line_position = [0] * max_lines
        self.line_length = [0] * max_lines
        self.total_line = 0

    def update(self, file_buffer):
        buffer = file_buffer.get_buffer()
        if not buffer:
            self.total_line = 0
            self.line_position[0] = 0
            self.line_length[0] = 0
            file_buffer.cursor_line = 0
            file_buffer.cursor_line_pos = 0
            return

        self.line_position[0] = 0
        self.line_length[0] = 0
        self.total_line = 0
        line_count = 0
        last_space_index = -1
        last_space_position = -1

        i = 0
        buffer_len = len(buffer)
        while i < buffer_len and self.total_line < self.max_lines - 1:
            c = buffer[i]
            line_count += 1

            if c == " ":
                last_space_index = i
                last_space_position = line_count

            # Handle words longer than cols
            if line_count == self.cols and last_space_index == -1:
                self.line_length[self.total_line] = line_count
                self.total_line += 1
                self.line_position[self.total_line] = i + 1
                line_count = 0
                last_space_index = -1
                last_space_position = -1
                i += 1
                continue

            # Newline or max chars
            if c == "\n" or line_count == self.cols:
                if c == "\n":
                    self.line_length[self.total_line] = line_count
                    self.total_line += 1
                    self.line_position[self.total_line] = i + 1
                    line_count = 0
                elif last_space_index != -1 and c != "\n":
                    self.line_length[self.total_line] = last_space_position
                    self.total_line += 1
                    self.line_position[self.total_line] = last_space_index + 1
                    line_count -= last_space_position
                else:
                    self.line_length[self.total_line] = line_count
                    self.total_line += 1
                    self.line_position[self.total_line] = i + 1
                    line_count = 0
                last_space_index = -1
                last_space_position = -1
            i += 1

        # Handle cursor position
        if file_buffer.cursor_pos >= buffer_len:
            file_buffer.cursor_pos = buffer_len - 1 if buffer_len > 0 else 0

        p_cursor_pos = file_buffer.cursor_pos
        file_buffer.cursor_line = 0
        file_buffer.cursor_line_pos = self.line_length[0]

        for idx in range(self.total_line, -1, -1):
            if p_cursor_pos >= self.line_position[idx]:
                file_buffer.cursor_line = idx
                file_buffer.cursor_line_pos = p_cursor_pos - self.line_position[idx]
                break