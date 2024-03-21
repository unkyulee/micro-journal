#include "display.h"
#include "app/app.h"
#include "service/SD/sd.h"

// TFT_eSPI setup
#include <TFT_eSPI.h>
#include <SPI.h>
#include <SD.h>

TFT_eSPI tft = TFT_eSPI(); // Invoke library, pins defined in User_Setup.h

#define MAX_CHARS 20
char text[MAX_CHARS + 1];
int pos = 0;
bool clear_screen = false;

void display_type(char key)
{
  text[pos++] = key;
  text[pos] = '\0';

  if (pos >= MAX_CHARS)
  {
    Serial.println("Writing to the file");
    writeFile(SD, "/test.txt", text);

    //
    pos = 0;
    text[pos] = '\0';
    clear_screen = true;
  }
}

void display_setup()
{
  // Initialise the TFT screen
  tft.begin();

  //
  tft.setRotation(3);

  // Fill screen with grey so we can see the effect of printing with and without
  // a background colour defined
  tft.fillScreen(TFT_BLACK);
}


bool blink = false;
void display_loop()
{
  if (clear_screen)
  {
    // clear screen
    tft.fillScreen(TFT_BLACK);
    clear_screen = false;
  }
  else
  {
    // https://github.com/Bodmer/TFT_eSPI/blob/master/examples/320%20x%20240/Free_Font_Demo/Free_Fonts.h
    tft.setFreeFont(&FreeMono9pt7b);

    // Set "cursor" at top left corner of display (0,0) and select font 2
    // (cursor will move to next line automatically during printing with 'tft.println'
    //  or stay on the line is there is room for the text with tft.print)
    tft.setCursor(0, 10);
    // Set the font colour to be white with a black background, set text size multiplier to 1
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(1);

    // We can now plot text on screen using the "print" class
    tft.println(text);

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
}
