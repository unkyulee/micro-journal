#include "WordProcessor.h"
#include "app/app.h"
#include "service/SD/sd.h"
#include "service/keyboard/keyboard.h"

#include <FS.h>
#include <SD.h>

//
#define FILENAME "/ujournal.txt"
#define TEXT_BUFFER_SIZE 1000
char text_buffer[TEXT_BUFFER_SIZE];
int text_pos = 0;
int text_last_save_pos = 0;

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

  // Initialize Word Processor
  // load file from the SD card
  WP_load_text();
}

///
void WP_loop() {}

void WP_load_text()
{
  app_log("Reading file: %s\n", FILENAME);
  File file = SD.open(FILENAME);
  if (!file)
  {
    app_log("Failed to open file for reading\n");
    return;
  }

  size_t fileSize = file.size();
  if (fileSize > 500)
  {
    // read the last 500 characters from the file
    if (!file.seek(-500, SeekEnd))
    {
      app_log("Failed to seek file pointer\n");
      file.close();
      return;
    }
  }

  // read the content and save to text_buffer
  int pos = 0;
  while (file.available())
  {
    text_buffer[pos++] = file.read();
  }
  // end with null
  text_buffer[pos] = '\0';
  text_pos = pos;

  //
  app_log("File loading completed\n");

  // close file
  file.close();
}

void WP_save_text()
{
  // save the current buffer to the file
  Serial.printf("Writing file: %s\n", FILENAME);

  File file = SD.open(FILENAME, FILE_WRITE);
  if (!file)
  {
    Serial.println("Failed to open file for writing");
    return;
  }
  if (file.print(text_buffer))
  {
    Serial.println("File written");
  }
  else
  {
    Serial.println("Write failed");
  }
  file.close();
}

//
bool blink = false;
bool clear = false;
int text_pos_prev;

//
void WP_render(TFT_eSPI *ptft)
{
  if (clear)
  {
    ptft->fillScreen(TFT_BLACK);
    clear = false;
  }

  // Set Font
  // https://github.com/Bodmer/TFT_eSPI/blob/master/examples/320%20x%20240/Free_Font_Demo/Free_Fonts.h
  ptft->setFreeFont(&FreeMono9pt7b);

  // remove the previous blink
  if (text_pos_prev != text_pos)
  {
    int cursorX = ptft->getCursorX();
    int cursorY = ptft->getCursorY();
    ptft->drawLine(cursorX, cursorY, cursorX + 10, cursorY, TFT_BLACK);

    // when backspace is pressed
    if (text_pos_prev > text_pos)
    {
      // remove the text
      int fontHeight = ptft->fontHeight();
      ptft->fillRect(cursorX - 10, cursorY - fontHeight + 4, 240, 320, TFT_BLACK);
      ptft->fillRect(0, cursorY + fontHeight + 4, 240, 320, TFT_BLACK);  

      app_log("x: %d y: %d h: %d\n", cursorX, cursorY, fontHeight);
    }

    // only when the new text is written from previous render
    text_pos_prev = text_pos;
  }

  // when using free font starting cursor needs to be 10 pixels down
  // to be visible the first line
  ptft->setCursor(0, 10);
  ptft->setTextColor(TFT_WHITE, TFT_BLACK);
  ptft->setTextSize(1);

  // Display the text
  ptft->print(text_buffer);

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

//
void WP_keyboard(char key)
{

  // if the text_pos reached the max
  if (text_pos >= TEXT_BUFFER_SIZE)
  {
    text_pos = 0;
  }

  if (key == BACKSPACE)
  {
    if (text_pos > 0)
    {
      // reduce the pos
      text_buffer[--text_pos] = '\0';
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

  // when enter key is pressed
  // save the buffer
  if (key == '\n')
  {
    WP_save_text();
  }
}

/*

if (clear_screen)
  {
    // clear screen
    tft.fillScreen(TFT_BLACK);
    clear_screen = false;
  }
  else
  {


    // Set "cursor" at top left corner of display (0,0) and select font 2
    // (cursor will move to next line automatically during printing with 'tft.println'
    //  or stay on the line is there is room for the text with tft.print)

    // Set the font colour to be white with a black background, set text size multiplier to 1


    // We can now plot text on screen using the "print" class


    // draw blinking cursor
    if (blink)
    {
      //
      tft.fillRect(10, 10, 10, 2, TFT_BLACK);
    }
    else
    {
      //
      tft.fillRect(10, 10, 10, 2, TFT_WHITE);
    }
  }

  static unsigned int last_blink = millis();
  if (millis() - last_blink > 1000)
  {
    last_blink = millis();
    blink = !blink;
  }

*/