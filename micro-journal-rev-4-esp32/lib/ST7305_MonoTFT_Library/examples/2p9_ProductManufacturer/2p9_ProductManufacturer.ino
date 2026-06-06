/*
 * ProductManufacturer.ino
 * Mirrors the behavior/layout of your main.cpp with your pin mapping.
 * Pins: DC=3, RST=6, CS=7, SCLK=2, SDIN=10
 */

#include <Arduino.h>
#include <SPI.h>
#include <ST73XX_UI.h>
#include <ST7305_2p9_BW_DisplayDriver.h>
#include <ST73xxPins.h>
#include <U8g2_for_ST73XX.h>
// Fallback pin mapping for this example (edit here to customize)
#ifndef PIN_DC
#define PIN_DC   4
#endif
#ifndef PIN_RST
#define PIN_RST  0
#endif
#ifndef PIN_CS
#define PIN_CS   3
#endif
#ifndef PIN_SCLK
#define PIN_SCLK 2
#endif
#ifndef PIN_SDIN
#define PIN_SDIN 1
#endif

// Fallback display size for this example
#ifndef DISPLAY_WIDTH
#define DISPLAY_WIDTH  168
#endif
#ifndef DISPLAY_HEIGHT
#define DISPLAY_HEIGHT 384
#endif



const ST73xxPins PINS{PIN_DC, PIN_CS, PIN_SCLK, PIN_SDIN, PIN_RST};
ST7305_2p9_BW_DisplayDriver display(PINS, SPI);
U8G2_FOR_ST73XX u8g2_for_st73xx;

void setup() {
  Serial.begin(115200);
  Serial.println("Hello monoTFT (ProductManufacturer)");

  // If your MCU needs explicit SPI pin binding (e.g., ESP32), keep this:
  SPI.begin(PIN_SCLK, -1, PIN_SDIN, PIN_CS);

  display.initialize();
  display.High_Power_Mode();
  display.display_on(true);
  display.display_Inversion(false);

  u8g2_for_st73xx.begin(display);   // hook u8g2 to the driver

  display.clearDisplay();
  display.display();
}

void loop() {
  long t0 = millis();

  // Outer border
  display.drawRectangle(2, 2, 166, 382, ST7305_COLOR_BLACK);

  // Text + shapes (same layout/content as main.cpp) :contentReference[oaicite:0]{index=0}
  u8g2_for_st73xx.setFontDirection(0);
  u8g2_for_st73xx.setForegroundColor(ST7305_COLOR_BLACK);
  u8g2_for_st73xx.setBackgroundColor(ST7305_COLOR_WHITE);

  // 中文行
  u8g2_for_st73xx.setFont(u8g2_font_wqy16_t_gb2312b);
  u8g2_for_st73xx.setCursor(5, 20);
  u8g2_for_st73xx.print(F("超低功耗"));

  // English line
  u8g2_for_st73xx.setFont(u8g2_font_helvR14_tf);
  u8g2_for_st73xx.setCursor(5, 40);
  u8g2_for_st73xx.print(F("Ultra-Low Power"));

  // Model + Chinese
  u8g2_for_st73xx.setFont(u8g2_font_wqy16_t_gb2312b);
  u8g2_for_st73xx.setCursor(5, 60);
  u8g2_for_st73xx.print(F("MonoTFT 2.9inch"));
  u8g2_for_st73xx.setCursor(5, 80);
  u8g2_for_st73xx.print(F("分辨率"));

  // Resolution
  u8g2_for_st73xx.setFont(u8g2_font_helvR14_tf);
  u8g2_for_st73xx.setCursor(51, 80);
  u8g2_for_st73xx.print(F("168X384Dots"));

  // Divider bar
  display.drawFilledRectangle(4, 88, 164, 98, ST7305_COLOR_BLACK);

  u8g2_for_st73xx.setCursor(2, 120);
  u8g2_for_st73xx.print(F(" Arduino Library "));
  // Feature bullets (Chinese)
  u8g2_for_st73xx.setFont(u8g2_font_wqy16_t_gb2312);
  u8g2_for_st73xx.setCursor(5, 140);
  u8g2_for_st73xx.print(F("【paper-like screen】"));
  u8g2_for_st73xx.setCursor(5, 160);
  u8g2_for_st73xx.print(F("【阳光下清晰可见】"));

  // Thin divider
  display.drawRectangle(4, 168, 164, 178, ST7305_COLOR_BLACK);

  // ASCII sample
  u8g2_for_st73xx.setFont(u8g2_font_helvR14_tf);
  u8g2_for_st73xx.setCursor(5, 198);
  u8g2_for_st73xx.print(F("0123456789ABCD"));

  // Shapes row
  display.drawFilledTriangle(30, 210, 5, 240, 55, 240, ST7305_COLOR_BLACK);
  display.drawCircle(84, 225, 15, ST7305_COLOR_BLACK);
  display.drawFilledRectangle(128, 210, 158, 240, ST7305_COLOR_BLACK);
  display.drawLine(5, 245, 164, 245, ST7305_COLOR_BLACK);

  // Symbols line
  u8g2_for_st73xx.setCursor(5, 262);
  u8g2_for_st73xx.print(F("~@#￥%……&*abcdefghi"));

  // Footer
  u8g2_for_st73xx.setFont(u8g2_font_wqy16_t_gb2312b);
  u8g2_for_st73xx.setCursor(8, 340);
  u8g2_for_st73xx.print(F("WWW.WLKLCD.COM"));
  u8g2_for_st73xx.setCursor(10, 360);
  u8g2_for_st73xx.print(F("+86 0755-32882855"));
  u8g2_for_st73xx.setCursor(5, 378);
  u8g2_for_st73xx.print(F("Display DEMO @ESP32"));

  // Big time readout
  u8g2_for_st73xx.setFont(u8g2_font_fub42_tn);
  u8g2_for_st73xx.setCursor(8, 320);
  u8g2_for_st73xx.printf("19:30");

  // Push to panel (normal, then inverted)
  display.display_Inversion(false);
  display.display();
  Serial.printf("display time = %ld ms\n", millis() - t0);
  delay(1000);

  display.display_Inversion(true);
  display.display();
  delay(1000);
}
