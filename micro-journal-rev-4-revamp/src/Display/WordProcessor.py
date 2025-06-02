import time
import displayio
from adafruit_display_text import label
import terminalio

from WriterDeck.Writer import writer


class WordProcessor:
    def __init__(self, display):
        self.display = display
        self.last_save_time = time.monotonic()
        self.last_sleep_time = time.monotonic()
        self.clear_background = True
        self.font_height = 18
        self.font_width = 12
        self.statusbar_y = 224
        self.screen_width = 160  # Adjust as needed
        self.screen_height = 80  # Adjust as needed

    def setup(self):
        # Load file and config if needed
        writer.load_file(writer.file_buffer.file_name or "0.txt")
        self.clear_background = True
        self.last_sleep_time = time.monotonic()

    def loop(self):
        self.check_saved()
        self.check_sleep()
        self.render_clear()
        self.render_text()
        self.render_status()
        self.render_cursor()

    def check_saved(self):
        # Auto-save after 4 seconds of inactivity
        buffer_size = writer.file_buffer.get_buffer_size()
        now = time.monotonic()
        if hasattr(self, 'last_buffer_size'):
            if self.last_buffer_size != buffer_size:
                self.last_save_time = now
                self.last_buffer_size = buffer_size
        else:
            self.last_buffer_size = buffer_size

        if now - self.last_save_time > 4:
            self.last_save_time = now
            if not writer.saved:
                writer.save_file()

    def check_sleep(self):
        # Go to sleep after 10 minutes of inactivity
        now = time.monotonic()
        if not writer.saved:
            self.last_sleep_time = now
        if now - self.last_sleep_time > 600:
            # Set your app's screen state to sleep here
            print("Entering sleep mode")
            self.last_sleep_time = now

    def render_clear(self):
        if self.clear_background:
            self.display.show(None)
            self.clear_background = False

    def render_text(self):
        # Render the visible lines of text
        group = displayio.Group()
        # Get lines from screen buffer
        lines = []
        buf = writer.file_buffer.get_buffer()
        sb = writer.screen_buffer
        total_lines = sb.total_line
        for i in range(total_lines + 1):
            start = sb.line_position[i]
            length = sb.line_length[i]
            lines.append(buf[start:start+length].replace('\n', ''))

        # Only show last N lines that fit the screen
        max_lines = min(len(lines), (self.screen_height - self.font_height) // self.font_height)
        visible_lines = lines[-max_lines:]

        for idx, line in enumerate(visible_lines):
            text_area = label.Label(
                terminalio.FONT,
                text=line,
                color=0xFFFFFF,
                x=2,
                y=2 + idx * self.font_height
            )
            group.append(text_area)

        self.display.show(group)

    def render_status(self):
        # Render a simple status bar at the bottom
        group = self.display.groups[0] if self.display.groups else displayio.Group()
        status_text = f"Saved: {'Yes' if writer.saved else 'No'} | Size: {writer.file_buffer.get_buffer_size()}"
        status_area = label.Label(
            terminalio.FONT,
            text=status_text,
            color=0x00FF00 if writer.saved else 0xFF0000,
            x=2,
            y=self.statusbar_y if self.statusbar_y < self.screen_height else self.screen_height - self.font_height
        )
        group.append(status_area)
        self.display.show(group)

    def render_cursor(self):
        # Render a blinking cursor at the current position
        # This is a simple implementation; you may want to draw a rectangle or line
        now = time.monotonic()
        blink = int(now * 2) % 2 == 0  # Blink every 0.5s
        if not blink:
            return

        sb = writer.screen_buffer
        fb = writer.file_buffer
        cursor_line = getattr(fb, 'cursor_line', 0)
        cursor_line_pos = getattr(fb, 'cursor_line_pos', 0)
        x = 2 + cursor_line_pos * self.font_width
        y = 2 + cursor_line * self.font_height

        # Draw a simple rectangle as cursor
        # You may need to use a TileGrid or custom drawing for this
        # For now, just print position for demonstration
        print(f"Cursor at ({x},{y})")

