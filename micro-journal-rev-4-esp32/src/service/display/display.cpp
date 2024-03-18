#include "display.h"
#include "app/app.h"

// TFT_eSPI setup
#include <TFT_eSPI.h>
#include <SPI.h>

TFT_eSPI tft = TFT_eSPI(); // Invoke library, pins defined in User_Setup.h

#define TFT_GREY 0x0AEB // New colour

//
void display_setup_main()
{
    tft.init();
    tft.setRotation(1);
}
void display_setup_secondary() {}

///
void display_loop_main()
{
    //Serial.println("Hi");

    // Fill screen with grey so we can see the effect of printing with and without
    // a background colour defined
    tft.fillScreen(TFT_GREY);

    // Set "cursor" at top left corner of display (0,0) and select font 2
    // (cursor will move to next line automatically during printing with 'tft.println'
    //  or stay on the line is there is room for the text with tft.print)
    tft.setCursor(0, 0, 2);
    // Set the font colour to be white with a black background, set text size multiplier to 1
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(1);
    // We can now plot text on screen using the "print" class
    tft.println("Hello World!");

    // Set the font colour to be yellow with no background, set to font 7
    tft.setTextColor(TFT_YELLOW);
    tft.setTextFont(2);
    tft.println(1234.56);

    // Set the font colour to be red with black background, set to font 4
    tft.setTextColor(TFT_RED, TFT_BLACK);
    tft.setTextFont(4);
    tft.println((long)3735928559, HEX); // Should print DEADBEEF

    // Set the font colour to be green with black background, set to font 2
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.setTextFont(2);
    tft.println("Groop");

    // Test some print formatting functions
    float fnumber = 123.45;
    // Set the font colour to be blue with no background, set to font 2
    tft.setTextColor(TFT_BLUE);
    tft.setTextFont(2);
    tft.print("Float = ");
    tft.println(fnumber); // Print floating point number
    tft.print("Binary = ");
    tft.println((int)fnumber, BIN); // Print as integer value in binary
    tft.print("Hexadecimal = ");
    tft.println((int)fnumber, HEX); // Print as integer number in Hexadecimal

}
void display_loop_secondary() {}