/* ======================
 * RainToSun_TextDemo.ino  (icon-safe)
 * ====================== */
#include <Arduino.h>
#include <SPI.h>
#include "ST7305_4p2_BW_DisplayDriver.h"
#include <ST73xxPins.h>
#include "U8g2_for_ST73XX.h"
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
#define DISPLAY_WIDTH  300
#endif
#ifndef DISPLAY_HEIGHT
#define DISPLAY_HEIGHT 400
#endif


const ST73xxPins PINS{PIN_DC, PIN_CS, PIN_SCLK, PIN_SDIN, PIN_RST};
ST7305_4p2_BW_DisplayDriver display(PINS, SPI);
U8G2_FOR_ST73XX u8g2;

// Safer set of symbols commonly present in u8g2 unifont symbol set
static const char* ROW_ICONS[10] = {
  "☀","☁","☂","❄","⚡","☑","✓","←","↑","→"
};

// Translations for "Monochrome Reflective Screen"
static const char* LANG_TEXTS[10] = {
  "Monochrome Reflective Screen",  // EN (0)
  "单色全反射屏幕",                // ZH (1)
  "मोनोक्रोम रिफ्लेक्टिव स्क्रीन", // HI (2)
  "Pantalla Reflectiva Monocroma", // ES (3)
  "شاشة عاكسة أحادية اللون",        // AR (4)
  "মনোক্রোম রিফ্লেকটিভ স্ক্রিন",     // BN (5)
  "Tela Reflexiva Monocromática",  // PT (6)
  "Монохромный отражающий экран",   // RU (7)
  "モノクロ反射型スクリーン",         // JA (8)
  "Écran Réfléchissant Monochrome" // FR (9)
};

static void setFontForLang(uint8_t idx) {
  switch(idx) {
    case 1: u8g2.setFont(u8g2_font_wqy16_t_gb2312b); break;          // Chinese
    case 8: u8g2.setFont(u8g2_font_unifont_t_japanese1); break;      // Japanese
    case 2: u8g2.setFont(u8g2_font_unifont_t_devanagari); break;     // Hindi
    case 4: u8g2.setFont(u8g2_font_unifont_t_arabic); break;         // Arabic (no shaping)
    case 5: u8g2.setFont(u8g2_font_unifont_t_bengali); break;        // Bengali
    case 7: u8g2.setFont(u8g2_font_unifont_t_cyrillic); break;       // Russian
    default: u8g2.setFont(u8g2_font_helvR14_tf); break;              // Latin languages
  }
}

// Draw an icon with fallback if glyph missing in current font
static void drawIconUTF8(int x, int y, const char* s) {
  u8g2.setFont(u8g2_font_unifont_t_symbols);
  // In u8g2, getUTF8Width returns pixel width for the current font
  int w = u8g2.getUTF8Width(s);
  if (w <= 0) {
    // fallback to ASCII star to guarantee visibility
    u8g2.setFont(u8g2_font_7x14B_mf);
    u8g2.drawUTF8(x, y, "*");
  } else {
    u8g2.drawUTF8(x, y, s);
  }
}

void setup() {
  SPI.begin(PIN_SCLK, -1, PIN_SDIN, PIN_CS);
  display.initialize(); display.High_Power_Mode();
  display.display_on(true); display.display_Inversion(false);
  u8g2.begin(display);
  u8g2.setFontMode(1);
  u8g2.setForegroundColor(ST7305_COLOR_BLACK);
  u8g2.setBackgroundColor(ST7305_COLOR_WHITE);
}

void loop() {
  display.clearDisplay();

  int y = 28;
  for (uint8_t i = 0; i < 10; ++i) {
    // Icon (with robust fallback)
    drawIconUTF8(6, y, ROW_ICONS[i]);

    // Native text
    setFontForLang(i);
    u8g2.drawUTF8(28, y, LANG_TEXTS[i]);

    // Line spacing (larger for complex scripts)
    y += (i==1 || i==2 || i==4 || i==5 || i==7 || i==8) ? 36 : 32;
  }

  display.display();
  delay(1000);
}
