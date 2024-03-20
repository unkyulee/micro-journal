#include "display.h"
#include "app/app.h"

//
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>

//
#define TFT_CS 21
#define TFT_DC 22
#define TFT_MOSI 23
#define TFT_MISO 19
#define TFT_CLK 18
#define TFT_RST -1

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);

//
void display_setup_main()
{
  delay(100);
  Serial.println("LCD LOADED");
  tft.begin();
  delay(100);

  // read diagnostics (optional but can help debug problems)
  uint8_t x = tft.readcommand8(ILI9341_RDMODE);
  Serial.print("Display Power Mode: 0x");
  Serial.println(x, HEX);
  x = tft.readcommand8(ILI9341_RDMADCTL);
  Serial.print("MADCTL Mode: 0x");
  Serial.println(x, HEX);
  x = tft.readcommand8(ILI9341_RDPIXFMT);
  Serial.print("Pixel Format: 0x");
  Serial.println(x, HEX);
  x = tft.readcommand8(ILI9341_RDIMGFMT);
  Serial.print("Image Format: 0x");
  Serial.println(x, HEX);
  x = tft.readcommand8(ILI9341_RDSELFDIAG);
  Serial.print("Self Diagnostic: 0x");
  Serial.println(x, HEX);

  tft.fillScreen(ILI9341_BLACK);
  unsigned long start = micros();
  tft.setCursor(0, 0);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(1);
  tft.println("Hello World!");
}
void display_setup_secondary()
{
}

///
void display_loop_main()
{
}

void display_loop_secondary()
{
  static unsigned int last = 0;
  if (millis() - last > 6000)
  {
    //
    last = millis();
  }
}