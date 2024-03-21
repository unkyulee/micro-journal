#include "WordProcessor.h"
#include "app/app.h"
#include "service/SD/sd.h"
#include "service/keyboard/keyboard.h"

#include <FS.h>
#include <SD.h>

//
#define FILENAME "/ujournal.txt"
#define TEXT_BUFFER_SIZE 2900 // buffer size should be divisible by row character count (29)
// #define TEXT_BUFFER_SIZE 348
char text_buffer[TEXT_BUFFER_SIZE + 2];
int text_pos = 0;
size_t text_last_save_pos = 0;
char *line_position[TEXT_BUFFER_SIZE / 13];

//
bool blink = false;
int text_pos_prev = -1;
int total_line_prev = -1;
int start_line_prev = 0;
bool clear = false;

//
void WP_setup()
{
  // when this is called
  // SD card should be ready
  JsonDocument &app = app_status();
  if (app["SD"].as<bool>() == false)
  {
    app_log("SD card is not ready. Not initializing WordProcessor\n");
  }

  //
  // Check if the file exists
  app_log("Check if file exists %s\n", FILENAME);
  if (!SD.exists(FILENAME))
  {
    // If the file doesn't exist, create it
    File file = SD.open(FILENAME, FILE_WRITE);
    if (!file)
    {
      Serial.println("Failed to create file");
      return;
    }
    file.close();
    Serial.println("File created");
  }

#ifdef DEBUG
  // always create a new file
  {
    // If the file doesn't exist, create it
    File file = SD.open(FILENAME, FILE_WRITE);
    if (!file)
    {
      Serial.println("Failed to create file");
      return;
    }
    file.close();
    Serial.println("File created");
  }
#endif

  // Initialize Word Processor
  // load file from the SD card
  WP_load_text();
}

///
void WP_loop() {}

void WP_load_text()
{
  app_log("Reading file: %s ... ", FILENAME);
  File file = SD.open(FILENAME);
  if (!file)
  {
    app_log("Failed to open file for reading\n");
    return;
  }

  size_t fileSize = file.size();
  if (fileSize > TEXT_BUFFER_SIZE / 2)
  {
    // read the last 500 characters from the file
    if (!file.seek(-TEXT_BUFFER_SIZE / 2, SeekEnd))
    {
      app_log("Failed to seek file pointer\n");
      file.close();
      return;
    }
    //
    text_last_save_pos = TEXT_BUFFER_SIZE / 2;
  }
  else
  {
    // set the buffer to the file size
    // when total size is smaller than the buffer
    text_last_save_pos = fileSize;
  }

  // read the content and save to text_buffer
  int pos = 0;
  text_buffer[pos] = '\0';
  while (file.available())
  {
    text_buffer[pos++] = file.read();
    text_buffer[pos] = '\0';
  }
  // end with null
  text_pos = pos;
  // refresh the writing line
  start_line_prev = 0;
  text_pos_prev = -1;

  //
  app_log("File loading completed: text_pos: %d\n", text_pos);

  // close file
  file.close();

  // debug print out entire text file
  /*
    {
      File file = SD.open(FILENAME);
      app_log("After saving file size is: %d\n", file.size());
      while (file.available())
      {
        Serial.print((char)file.read());
      }
    }
  */
}

void WP_save_text()
{
  // debounce 10 seconds
  // too frequent save can break the SD communication
  static unsigned int last = millis();
  if (millis() - last > 10000)
  {
    last = millis();

    //// save the current buffer to the file
    app_log("Writing file: %s ...", FILENAME);
    File file = SD.open(FILENAME, FILE_APPEND);
    if (!file)
    {
      app_log("Failed to open file for writing\n");
      return;
    }
    if (file.print(&text_buffer[text_last_save_pos]))
    {
      app_log("File written from %d\n", text_last_save_pos);
      text_last_save_pos = text_pos;
    }
    else
    {
      app_log("Write failed\n");
    }
    file.close();
  }
}

//
void WP_keyboard(char key)
{
  if (key == BACKSPACE)
  {
    if (text_pos > 0)
    {
      // reduce the pos
      text_buffer[--text_pos] = '\0';

      // refresh the screen
      clear = true;
    }
    else
    {
      // reached the beginning of the buffer
      // load previous contents from the file
    }
  }
  else
  {
    text_buffer[text_pos++] = key;
    text_buffer[text_pos] = '\0';
  }

  // if the text_pos reached the max
  if (text_pos >= TEXT_BUFFER_SIZE)
  {
    // buffer is about to overflow.
    // save the file then reload
    WP_save_text();

    delay(100);

    //
    WP_load_text();

    //
    clear = true;
  }

  // app_log("key: %c text_pos: %d\n", key, text_pos);
}

//
void WP_render(TFT_eSPI *ptft)
{
  // clear screen
  if (clear)
  {
    clear = false;
    // refresh the screen
    ptft->fillScreen(TFT_BLACK);
  }

  // in case when a new character is entered or backspace comes.
  // remove the previous carrot blink
  if (text_pos_prev != text_pos)
  {
    int cursorX = ptft->getCursorX();
    int cursorY = ptft->getCursorY();
    ptft->drawLine(cursorX, cursorY, cursorX + 10, cursorY, TFT_BLACK);

    // only when the new text is written from previous render
    text_pos_prev = text_pos;
  }

  // when displaying the text, it should make that the cursor is always displayed at the bottom line
  // 320 x 240 space with 24 height characters
  // total 13 rows and 29 in the columns
  // 377 in one screen
  // new line keys will be counted as a line
  int total_line = 0;
  int row_character_count = 0;
  char last_char;
  const int max_row_characters = 29;
  const int max_lines = 10;

  // first line will be always the same
  line_position[0] = &text_buffer[0];

  for (int i = 0; i < TEXT_BUFFER_SIZE; i++)
  {
    // scan all the text to calculate the line
    if (text_buffer[i] == '\0')
      break; // break when text ends

    // if at the beginning of the line, then space doesn't get counted
    if (row_character_count == 0 && text_buffer[i] == ' ')
    {
      // start of a line is a space is not rendered correctly
      // so it should skip to the next character as the start of the line
      line_position[total_line] = &text_buffer[i + 1];
      continue;
    }

    // new character in the line
    row_character_count++;

    // start of the new line has been identified
    if (text_buffer[i] == '\n' || row_character_count == max_row_characters)
    {
      line_position[++total_line] = &text_buffer[i + 1];
      row_character_count = 0;
    }

    //
    last_char = text_buffer[i];
  }

  // text should be displayed (total_line - total available line count)
  int start_line = start_line_prev;

  // move the start line if it reached the last line
  if (total_line - start_line_prev >= max_lines)
  {
    start_line = total_line - 2;
    if (start_line < 0)
      start_line = 0;

    //
    clear = true;

    start_line_prev = start_line;
  }

  // refresh the screen if the line count has changed
  if (total_line_prev != total_line)
  {
    // save file whenever new line is added
    if (total_line_prev >= 0)
      WP_save_text();

    //
    total_line_prev = total_line;
  }

  // Write debug status bar
  /*
  ptft->setCursor(0, 0, 2);
  ptft->setTextColor(TFT_WHITE, TFT_RED);
  ptft->printf("TL: %d, SL: %d, R: %d, T: %d\n", total_line, start_line, row_character_count, text_pos);
  */

  // https://github.com/Bodmer/TFT_eSPI/blob/master/examples/320%20x%20240/Free_Font_Demo/Free_Fonts.h
  ptft->setFreeFont(&FreeMono9pt7b);
  ptft->setCursor(0, 36);
  ptft->setTextColor(TFT_WHITE, TFT_BLACK);
  ptft->setTextSize(1);

  // Display the text
  ptft->print(line_position[start_line]);

  // when the line is full then print out a new line
  // so that blinking carot goes bottom line
  if (row_character_count == 0 && total_line > 0 && last_char != '\n')
  {
    ptft->println("");
  }

  // display the blink key
  static unsigned int last = millis();
  if (millis() - last > 500)
  {
    last = millis();

    // toggle cursor blink every second
    blink = !blink;
  }

  int cursorX = ptft->getCursorX();
  int cursorY = ptft->getCursorY();
  if (blink)
  {
    // Draw a white line
    ptft->drawLine(cursorX, cursorY, cursorX + 10, cursorY, TFT_WHITE);
  }
  else
  {
    // Draw a black line
    ptft->drawLine(cursorX, cursorY, cursorX + 10, cursorY, TFT_BLACK);
  }
}
