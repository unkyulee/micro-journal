/*
 * 4p2_calendarAnimation.ino — ST7305 4.2" (native 300×400) rendered as LANDSCAPE 400×300
 * - Background stays white; we draw BLACK content only (no white fills after clear).
 * - Wi-Fi/IP are NOT drawn (Wi-Fi still used for time/weather fetch).
 *
 * Modes:
 *   powerSavingMode == false:
 *       - WiFi always ON
 *       - Show seconds
 *       - Refresh every second
 *       - Weather/Sun refreshed hourly (without turning WiFi off)
 *
 *   powerSavingMode == true:
 *       - WiFi OFF except for hourly sync
 *       - Hide seconds
 *       - Refresh every minute
 *       - Every hour: briefly turn WiFi ON to sync time+weather, then OFF immediately
 */

#include <Arduino.h>
#include <WiFi.h>
#include <time.h>
#include <SPI.h>
#include "ST7305_4p2_BW_DisplayDriver.h"
#include <ST73xxPins.h>
#include "U8g2_for_ST73XX.h"
#include <pgmspace.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <stdint.h>

// ================= USER SETTINGS =================

#define WIFI_SSID "WIFI_SSID"
#define WIFI_PASSWORD "WIFI_SSID"


// Region selection: 0=Auto, 1=Singapore, 2=South Korea
#define REGION_MODE 1

// false: WiFi ON + seconds + 1s refresh
// true : WiFi OFF except hourly sync + no seconds + 1min refresh
bool powerSavingMode = false;

// Optional: flip if your specific panel is inverted
#define PANEL_INVERT 0  // 0 = normal (black on white), 1 = inverted panel

// ================= Pins ==========================
#define PIN_DC   4
#define PIN_CS   3
#define PIN_SCLK 2
#define PIN_SDIN 1
#define PIN_RST  0

// ================= Weather icon TYPE ==============
enum class WeatherIconType : uint8_t { Clear, Partly, Cloudy, Rain, Snow, Fog, Storm };

// ================= Display & UI ==================
static const int DEV_W = 300;  // driver/native
static const int DEV_H = 400;
static const int LAND_W = 400;  // logical landscape canvas
static const int LAND_H = 300;

const ST73xxPins PINS{ PIN_DC, PIN_CS, PIN_SCLK, PIN_SDIN, PIN_RST };
ST7305_4p2_BW_DisplayDriver display(PINS, SPI);
U8G2_FOR_ST73XX u8g2;

static inline uint16_t COL_FG() { return ST7305_COLOR_BLACK; }

// ---- Fonts: ONLY TWO ----
#define FONT_BIG   u8g2_font_helvB24_te
#define FONT_SMALL u8g2_font_helvB10_te

// Layout constants
static const int MARGIN = 8;
static const int HEADER_BOTTOM = 120;   // compact header so calendar starts higher
static const int URL_LINE_SP = 16;
static const int URL_BLOCK_H = 2 * URL_LINE_SP + 2;

// Left header (SMALL font items)
static const int Y_DATE_SMALL     = 24;  // "Mon YYYY" (not used; we draw Month Year with big font)
static const int Y_ICON_TOP       = 46;  // 24×24 icon
static const int Y_WEATHER_SMALL  = 66;  // weather text baseline
static const int Y_WEATHER2_SMALL = Y_WEATHER_SMALL + 14; // temp + humidity line

// Right header
static const int Y_TIME_BIG = 40;   // big time baseline
static const int Y_SUN_SMALL = 76;  // sunrise/set small

// Bottom URL
static const char* URL_TEXT =
  " github.com ---------> ST7305_MonoTFT_Library ---------> ST7305_MonoTFT_Library";

// ================= Time/geo/weather ==================
static bool haveTime = false;
static long g_tzOffsetSec = 0;
static int lastDrawSec = -1;
static int lastDrawMin = -1;
static int lastDrawDay = -1;

static double g_lat = NAN, g_lon = NAN;
static String g_tzIANA = "";

static float g_tempC = NAN;
static float g_humidity = NAN;
static int g_wcode = -1;
static String g_sunrise = "--:--";
static String g_sunset  = "--:--";
static unsigned long g_lastWeatherMs = 0;
static const unsigned long WEATHER_INTERVAL_MS = 60UL * 60UL * 1000UL; // hourly

// ================= Region presets ===================
struct RegionPreset {
  const char* name;
  const char* tzIANA;
  double lat, lon;
  long tzOffset;  // seconds
  const char* ntpServer;
};
static const RegionPreset PRESET_SG = { "Singapore", "Asia/Singapore", 1.3521, 103.8198, 8 * 3600, "sg.pool.ntp.org" };
static const RegionPreset PRESET_KR = { "South Korea", "Asia/Seoul",    37.5665, 126.9780, 9 * 3600, "kr.pool.ntp.org" };
static const RegionPreset* gPreset = nullptr;

// ================= QR (49×49) =======================
// Auto-generated: QR for URL_TEXT
// Modules: 49x49 (incl. quiet zone); Bytes/row: 7; Total bytes: 343
const uint8_t QR_URL[] PROGMEM = {
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,0xEE,0xFA,0x46,
  0xCB,0xF8,0x00,0x08,0x20,0xEB,0x6E,0xDA,0x08,0x00,0x0B,0xA1,0xDE,0x3A,0x0A,0xE8,
  0x00,0x0B,0xAF,0x8D,0x58,0x62,0xE8,0x00,0x0B,0xAD,0x48,0x54,0xA2,0xE8,0x00,0x08,
  0x2C,0x29,0x6A,0x52,0x08,0x00,0x0F,0xEA,0xAA,0xAA,0xAB,0xF8,0x00,0x00,0x0D,0x1F,
  0xC0,0x68,0x00,0x00,0x08,0xBF,0xFC,0xAA,0x4F,0xC8,0x00,0x02,0xD5,0x92,0x1F,0xE3,
  0xD8,0x00,0x09,0x3B,0x38,0x3B,0x2C,0x08,0x00,0x07,0x00,0xBC,0xF9,0x7B,0xD0,0x00,
  0x0F,0xF1,0x9D,0xF4,0x45,0x00,0x00,0x00,0x4A,0x03,0x08,0xBE,0xE8,0x00,0x0E,0xAC,
  0x71,0x1A,0xAE,0xF8,0x00,0x07,0x1A,0xAD,0xF2,0x61,0x50,0x00,0x0A,0xEC,0x44,0xA1,
  0xF1,0x20,0x00,0x04,0xCA,0x34,0x22,0x97,0x90,0x00,0x01,0xB8,0x62,0x7F,0x4F,0x68,
  0x00,0x0C,0x54,0x78,0xCD,0x77,0x40,0x00,0x02,0x28,0x88,0x9C,0x38,0x58,0x00,0x0D,
  0x54,0x9E,0xB2,0xD7,0xC8,0x00,0x0A,0x22,0x96,0x9F,0xC9,0xC8,0x00,0x08,0x41,0x4C,
  0x71,0x7D,0x90,0x00,0x04,0xED,0x64,0x38,0xC8,0x50,0x00,0x09,0x03,0xBE,0x33,0xCB,
  0xB8,0x00,0x07,0x37,0x3E,0xFF,0xAE,0x08,0x00,0x01,0x19,0xF4,0xCB,0xDF,0x48,0x00,
  0x00,0x7A,0x04,0x7D,0xC9,0x00,0x00,0x08,0x43,0x3B,0xA2,0x30,0xA8,0x00,0x03,0xFD,
  0x10,0xC7,0x14,0x88,0x00,0x00,0xD5,0xCC,0x6B,0xFC,0x00,0x00,0x0F,0x76,0x15,0xB8,
  0xEF,0x98,0x00,0x00,0x0E,0x5B,0x77,0x48,0x88,0x00,0x0F,0xEF,0xEC,0x91,0xAA,0xD8,
  0x00,0x08,0x26,0xC7,0xAB,0x98,0xC8,0x00,0x0B,0xAD,0xBA,0x3E,0x9F,0xD0,0x00,0x0B,
  0xA5,0x60,0x14,0xA1,0x10,0x00,0x0B,0xA3,0x46,0x55,0x7C,0x28,0x00,0x08,0x25,0x3F,
  0x7A,0x53,0x58,0x00,0x0F,0xEB,0x8B,0xD9,0x4D,0x90,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,
};
const int QR_URL_W = 49, QR_URL_H = 49, QR_URL_ROWBYTES = 7;

// =============== Rotation helpers (land → dev) ===============
static inline void land2dev(int x_land, int y_land, int& xd, int& yd) {
  xd = y_land;
  yd = DEV_H - 1 - x_land;
}
static inline void drawRectL(int x1, int y1, int x2, int y2, uint16_t col) {
  int ax, ay, bx, by;
  land2dev(x1, y1, ax, ay);
  land2dev(x2, y2, bx, by);
  if (ax > bx) { int t = ax; ax = bx; bx = t; }
  if (ay > by) { int t = ay; ay = by; by = t; }
  display.drawRectangle(ax, ay, bx, by, col);
}
static inline void fillRectL(int x1, int y1, int x2, int y2, uint16_t col) {
  int ax, ay, bx, by;
  land2dev(x1, y1, ax, ay);
  land2dev(x2, y2, bx, by);
  if (ax > bx) { int t = ax; ax = bx; bx = t; }
  if (ay > by) { int t = ay; ay = by; by = t; }
  display.drawFilledRectangle(ax, ay, bx, by, col);
}
static inline void drawLineL(int x1, int y1, int x2, int y2, uint16_t col) {
  int ax, ay, bx, by;
  land2dev(x1, y1, ax, ay);
  land2dev(x2, y2, bx, by);
  display.drawLine(ax, ay, bx, by, col);
}
static inline void u8_begin_landscape() {
  u8g2.begin(display);
  u8g2.setFontMode(1);                          // transparent text
  u8g2.setForegroundColor(ST7305_COLOR_BLACK);  // content = black
  u8g2.setBackgroundColor(ST7305_COLOR_WHITE);  // background = white
  u8g2.setFontDirection(3);                     // along +X_land
}
static inline int textWidthL(const char* s) { return u8g2.getUTF8Width(s); }
static inline void drawTextL(int x, int y, const char* s) {
  int xd, yd; land2dev(x, y, xd, yd); u8g2.drawUTF8(xd, yd, s);
}

// ======== 1px pixel + circle (for today highlight) =========
static inline void drawPixelL(int x, int y, uint16_t col) { drawLineL(x, y, x, y, col); }
static void drawCircleL(int cx, int cy, int r, uint16_t col) {
  int f = 1 - r, ddF_x = 1, ddF_y = -2 * r, x = 0, y = r;
  drawPixelL(cx, cy + r, col); drawPixelL(cx, cy - r, col);
  drawPixelL(cx + r, cy, col); drawPixelL(cx - r, cy, col);
  while (x < y) {
    if (f >= 0) { y--; ddF_y += 2; f += ddF_y; }
    x++; ddF_x += 2; f += ddF_x;
    drawPixelL(cx + x, cy + y, col); drawPixelL(cx - x, cy + y, col);
    drawPixelL(cx + x, cy - y, col); drawPixelL(cx - x, cy - y, col);
    drawPixelL(cx + y, cy + x, col); drawPixelL(cx - y, cy + x, col);
    drawPixelL(cx + y, cy - x, col); drawPixelL(cx - y, cy - x, col);
  }
}

// ================= Helpers ===================
static inline int iRound(float v) { return (v >= 0) ? int(v + 0.5f) : int(v - 0.5f); }

// =============== WiFi / Time helpers =========================
static void wifiOn() { WiFi.mode(WIFI_STA); WiFi.begin(WIFI_SSID, WIFI_PASSWORD); }
static void wifiWait(uint32_t ms = 20000) {
  uint32_t t0 = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - t0 < ms) delay(250);
}
static void wifiOff() { WiFi.disconnect(true, true); WiFi.mode(WIFI_OFF); }
static bool getLocalTm(struct tm& out) {
  time_t now; time(&now);
  time_t local = now + g_tzOffsetSec;
  return gmtime_r(&local, &out);
}

// Regional NTP (for SG/KR modes)
static bool syncTimeViaNTP(const char* ntpServer) {
  configTime(0, 0, ntpServer);
  time_t now = 0;
  for (int i = 0; i < 50; ++i) { delay(200); time(&now); if (now > 1700000000) break; }
  haveTime = (now > 1700000000);
  return haveTime;
}

// Auto-IP providers (only used when REGION_MODE==0)
static bool fetchIPGeo(double& lat, double& lon, String& tz) {
  WiFiClientSecure c; c.setInsecure();
  HTTPClient http;
  if (!http.begin(c, "https://ipapi.co/json")) return false;
  int code = http.GET();
  if (code != HTTP_CODE_OK) { http.end(); return false; }
  String body = http.getString(); http.end();
  int p = body.indexOf("\"latitude\":"); if (p < 0) return false; lat = body.substring(p + 11).toFloat();
  p = body.indexOf("\"longitude\":"); if (p < 0) return false; lon = body.substring(p + 12).toFloat();
  p = body.indexOf("\"timezone\":\""); if (p < 0) return false;
  int s = p + 12, e = body.indexOf("\"", s); if (e < 0) return false; tz = body.substring(s, e);
  return true;
}
static bool fetchWorldTime(long& tzOff) {
  WiFiClientSecure c; c.setInsecure();
  HTTPClient http;
  if (!http.begin(c, "https://worldtimeapi.org/api/ip")) return false;
  int code = http.GET();
  if (code != HTTP_CODE_OK) { http.end(); return false; }
  String b = http.getString(); http.end();
  int p = b.indexOf("\"unixtime\":"); if (p < 0) return false; long uni = b.substring(p + 11).toInt();
  p = b.indexOf("\"raw_offset\":"); if (p < 0) return false; long ro = b.substring(p + 13).toInt();
  p = b.indexOf("\"dst_offset\":"); if (p < 0) return false; long dso = b.substring(p + 13).toInt();
  tzOff = ro + dso;
  struct timeval tv { .tv_sec = uni, .tv_usec = 0 }; settimeofday(&tv, nullptr);
  haveTime = true; return true;
}

// =============== Weather + Sunrise/Sunset =====================
static String weatherCondFromCode(int code) {
  if (code == 0) return "Clear";
  if (code == 1 || code == 2) return "Partly";
  if (code == 3) return "Cloudy";
  if (code == 45 || code == 48) return "Fog";
  if ((code >= 51 && code <= 67) || (code >= 80 && code <= 82)) return "Rain";
  if (code >= 71 && code <= 77) return "Snow";
  if (code == 95 || code == 96 || code == 99) return "Storm";
  return "---";
}
static bool fetchWeatherAndSun() {
  if (isnan(g_lat) || isnan(g_lon) || !g_tzIANA.length()) return false;

  // Prefer "current" (has humidity)
  char url[512];
  snprintf(url, sizeof(url),
           "https://api.open-meteo.com/v1/forecast?latitude=%.4f&longitude=%.4f&current=temperature_2m,relative_humidity_2m,weather_code&daily=sunrise,sunset&timezone=%s",
           (double)g_lat, (double)g_lon, g_tzIANA.c_str());

  WiFiClientSecure c; c.setInsecure();
  HTTPClient http;
  if (!http.begin(c, url)) return false;
  int code = http.GET();
  if (code != HTTP_CODE_OK) { http.end(); return false; }
  String body = http.getString(); http.end();

  // Parse "current"
  int pc = body.indexOf("\"current\"");
  bool gotCurrent = false;
  if (pc >= 0) {
    int pt = body.indexOf("\"temperature_2m\":", pc);
    int ph = body.indexOf("\"relative_humidity_2m\":", pc);
    int pw = body.indexOf("\"weather_code\":", pc);
    if (pt > 0 && pw > 0) {
      g_tempC = body.substring(pt + 17).toFloat();
      g_wcode = body.substring(pw + 15).toInt();
      if (ph > 0) g_humidity = body.substring(ph + 24).toFloat();
      gotCurrent = true;
    }
  }

  if (!gotCurrent) {
    // Fallback: "current_weather" (no humidity)
    snprintf(url, sizeof(url),
             "https://api.open-meteo.com/v1/forecast?latitude=%.4f&longitude=%.4f&current_weather=true&daily=sunrise,sunset&timezone=%s",
             (double)g_lat, (double)g_lon, g_tzIANA.c_str());
    if (!http.begin(c, url)) return false;
    code = http.GET();
    if (code != HTTP_CODE_OK) { http.end(); return false; }
    body = http.getString(); http.end();
    int pCW = body.indexOf("\"current_weather\"");
    if (pCW >= 0) {
      int pT = body.indexOf("\"temperature\":", pCW);
      if (pT > 0) g_tempC = body.substring(pT + 14).toFloat();
      int pC = body.indexOf("\"weathercode\":", pCW);
      if (pC > 0) g_wcode = body.substring(pC + 14).toInt();
      g_humidity = NAN;
    }
  }

  // Parse daily sunrise/sunset
  int pDaily = body.indexOf("\"daily\"");
  if (pDaily >= 0) {
    int ps = body.indexOf("\"sunrise\":[", pDaily);
    if (ps > 0) {
      int s1 = body.indexOf("\"", ps + 11), s2 = body.indexOf("\"", s1 + 1);
      if (s1 > 0 && s2 > s1) {
        String ts = body.substring(s1 + 1, s2);
        int T = ts.indexOf('T');
        g_sunrise = (T > 0 && ts.length() >= T + 5) ? ts.substring(T + 1, T + 6) : "--:--";
      }
    }
    int pss = body.indexOf("\"sunset\":[", pDaily);
    if (pss > 0) {
      int s1 = body.indexOf("\"", pss + 10), s2 = body.indexOf("\"", s1 + 1);
      if (s1 > 0 && s2 > s1) {
        String ts = body.substring(s1 + 1, s2);
        int T = ts.indexOf('T');
        g_sunset = (T > 0 && ts.length() >= T + 5) ? ts.substring(T + 1, T + 6) : "--:--";
      }
    }
  }

  g_lastWeatherMs = millis();
  return true;
}

// =============== Weather icon helpers ========================
static WeatherIconType weatherIconTypeFromCode(int code) {
  if (code == 0) return WeatherIconType::Clear;
  if (code == 1 || code == 2) return WeatherIconType::Partly;
  if (code == 3) return WeatherIconType::Cloudy;
  if ((code >= 51 && code <= 67) || (code >= 80 && code <= 82)) return WeatherIconType::Rain;
  if (code >= 71 && code <= 77) return WeatherIconType::Snow;
  if (code == 45 || code == 48) return WeatherIconType::Fog;
  if (code == 95 || code == 96 || code == 99) return WeatherIconType::Storm;
  return WeatherIconType::Cloudy;
}

// 24×24 icon at (x,y)
static void drawWeatherIcon_L(int x, int y, WeatherIconType t) {
  int cx = x + 12, cy = y + 12;
  switch (t) {
    case WeatherIconType::Clear:
      fillRectL(cx - 3, cy - 3, cx + 3, cy + 3, COL_FG());
      drawLineL(cx - 10, cy, cx - 4, cy, COL_FG());
      drawLineL(cx + 4, cy, cx + 10, cy, COL_FG());
      drawLineL(cx, cy - 10, cx, cy - 4, COL_FG());
      drawLineL(cx, cy + 4, cx, cy + 10, COL_FG());
      drawLineL(cx - 7, cy - 7, cx - 4, cy - 4, COL_FG());
      drawLineL(cx + 4, cy + 4, cx + 7, cy + 7, COL_FG());
      drawLineL(cx - 7, cy + 7, cx - 4, cy + 4, COL_FG());
      drawLineL(cx + 4, cy - 4, cx + 7, cy - 7, COL_FG());
      break;
    case WeatherIconType::Partly:
      fillRectL(x + 2, y + 2, x + 6, y + 6, COL_FG());
      drawLineL(x + 1, y + 1, x + 7, y + 1, COL_FG());
      drawLineL(x + 1, y + 1, x + 1, y + 7, COL_FG());
      fillRectL(x + 6, y + 12, x + 18, y + 16, COL_FG());
      fillRectL(x + 8, y + 10, x + 16, y + 12, COL_FG());
      break;
    case WeatherIconType::Cloudy:
      fillRectL(x + 5, y + 10, x + 19, y + 14, COL_FG());
      fillRectL(x + 8, y + 8, x + 16, y + 10, COL_FG());
      fillRectL(x + 2, y + 12, x + 22, y + 16, COL_FG());
      break;
    case WeatherIconType::Rain:
      fillRectL(x + 5, y + 8, x + 19, y + 12, COL_FG());
      fillRectL(x + 3, y + 12, x + 21, y + 15, COL_FG());
      drawLineL(x + 8, y + 17, x + 6, y + 22, COL_FG());
      drawLineL(x + 13, y + 17, x + 11, y + 22, COL_FG());
      drawLineL(x + 18, y + 17, x + 16, y + 22, COL_FG());
      break;
    case WeatherIconType::Snow:
      fillRectL(x + 5, y + 8, x + 19, y + 12, COL_FG());
      fillRectL(x + 3, y + 12, x + 21, y + 15, COL_FG());
      drawLineL(cx - 2, y + 18, cx + 2, y + 22, COL_FG());
      drawLineL(cx + 2, y + 18, cx - 2, y + 22, COL_FG());
      drawLineL(cx, y + 17, cx, y + 23, COL_FG());
      break;
    case WeatherIconType::Fog:
      drawLineL(x + 4, y + 12, x + 20, y + 12, COL_FG());
      drawLineL(x + 3, y + 15, x + 21, y + 15, COL_FG());
      drawLineL(x + 5, y + 18, x + 19, y + 18, COL_FG());
      break;
    case WeatherIconType::Storm:
      fillRectL(x + 5, y + 8, x + 19, y + 12, COL_FG());
      fillRectL(x + 3, y + 12, x + 21, y + 15, COL_FG());
      drawLineL(cx - 3, y + 16, cx, y + 16, COL_FG());
      drawLineL(cx, y + 16, cx - 4, y + 23, COL_FG());
      drawLineL(cx - 4, y + 23, cx + 2, y + 23, COL_FG());
      break;
  }
}

// =============== URL draw (transparent) =======================
static void drawWrappedUrlCentered_L(int yTop, int lineSpacing, int maxWidth) {
  u8_begin_landscape();
  u8g2.setFont(FONT_SMALL);
  String text(URL_TEXT);
  int w = textWidthL(text.c_str());
  if (w <= maxWidth) {
    int x = (LAND_W - w) / 2;
    drawTextL(x, yTop, text.c_str());
  } else {
    int len = text.length(), half = len / 2, best = -1, bestDelta = 1 << 30;
    for (int i = 1; i < len - 1; ++i) {
      char ch = text[i];
      if (ch == '/' || ch == '-' || ch == '_' || ch == '?') {
        String L1 = text.substring(0, i), L2 = text.substring(i);
        if (textWidthL(L1.c_str()) <= maxWidth && textWidthL(L2.c_str()) <= maxWidth) { best = i; break; }
        int d = abs(i - half);
        if (d < bestDelta) { bestDelta = d; best = i; }
      }
    }
    if (best < 0) best = half;
    String L1 = text.substring(0, best), L2 = text.substring(best);
    int w1 = textWidthL(L1.c_str()), w2 = textWidthL(L2.c_str());
    drawTextL((LAND_W - w1) / 2, yTop, L1.c_str());
    drawTextL((LAND_W - w2) / 2, yTop + lineSpacing, L2.c_str());
  }
}

// =============== Header (LEFT info, RIGHT time) ===============
static int drawHeaderSplit_L(const struct tm& t, int rightX, int rightW, int /*qrPix*/, bool showSeconds) {
  u8_begin_landscape();

  // LEFT: Month + Year (BIG font)
  static const char* MTH[12] = { "Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec" };
  char head[32];
  snprintf(head, sizeof(head), "%s %d", MTH[t.tm_mon], 1900 + t.tm_year);
  u8g2.setFont(FONT_BIG);
  drawTextL(25, 30, head);

  // ===== WEATHER LAYOUT =====
  // First line: [ICON][gap][condition]
  const int iconW = 24;
  const int gap = 4; // tight spacing so icon & text feel together
  String cond = weatherCondFromCode(g_wcode);
  u8g2.setFont(FONT_SMALL);
  int leftAreaW = LAND_W - rightW - 2 * MARGIN;
  int condMaxW = max(40, leftAreaW - (iconW + gap));
  String condTrim = cond;
  while (textWidthL(condTrim.c_str()) > condMaxW && condTrim.length() > 0) condTrim.remove(condTrim.length() - 1);

  drawWeatherIcon_L(MARGIN, Y_ICON_TOP, weatherIconTypeFromCode(g_wcode));
  drawTextL(MARGIN + iconW + gap, Y_WEATHER_SMALL, condTrim.c_str());

  // Second line: temperature + humidity
  String th = "";
  if (!isnan(g_tempC))         th += String(iRound(g_tempC)) + " Celsius ";
  if (!isnan(g_humidity)) { if (th.length()) th += "  "; th += String(iRound(g_humidity)) + "% Humid"; }
  if (th.length()) {
    int thMaxW = leftAreaW;
    String thTrim = th;
    while (textWidthL(thTrim.c_str()) > thMaxW && thTrim.length() > 0) thTrim.remove(thTrim.length() - 1);
    drawTextL(MARGIN, Y_WEATHER2_SMALL+2, thTrim.c_str());
  }
  // ===== END WEATHER LAYOUT =====

  // RIGHT: Big 24h time
  char timeStr[16];
  if (showSeconds) snprintf(timeStr, sizeof(timeStr), "%02d:%02d:%02d", t.tm_hour, t.tm_min, t.tm_sec);
  else              snprintf(timeStr, sizeof(timeStr), "%02d:%02d",    t.tm_hour, t.tm_min);

  u8g2.setFont(FONT_BIG);
  int tw = textWidthL(timeStr);
  int timeX = rightX + (rightW - tw) / 2;
  if (timeX < rightX) timeX = rightX;
  drawTextL(timeX, Y_TIME_BIG, timeStr);

  // RIGHT: Sunrise/Sunset (SMALL)
  u8g2.setFont(FONT_SMALL);
  String sun = String("Rise ") + g_sunrise + "  Set " + g_sunset;
  int sw = textWidthL(sun.c_str());
  int sunX = rightX + (rightW - sw) / 2 - 10;
  if (sunX < rightX) sunX = rightX;
  drawTextL(sunX, Y_SUN_SMALL, sun.c_str());

  return HEADER_BOTTOM;
}

// =============== Calendar (black lines/text only) =============
static int daysInMonth(int y, int m) {
  struct tm t = { 0 };
  t.tm_year = y - 1900; t.tm_mon = m + 1; t.tm_mday = 0;
  mktime(&t);
  return t.tm_mday;
}
static int wdayOfFirst(int y, int m) {
  struct tm t = { 0 };
  t.tm_year = y - 1900; t.tm_mon = m; t.tm_mday = 1;
  mktime(&t);
  return t.tm_wday;
}
static void drawCalendar_L(const struct tm& t, int x0, int y0, int availW, int availH) {
  static const char* WD[] = { "Sun","Mon","Tue","Wed","Thu","Fri","Sat" };
  int cellW = max(20, (availW / 7));
  int calW = cellW * 7;
  int labelH = 12;
  int cellH = max(20, (availH - labelH - 10) / 6);

  u8_begin_landscape();
  u8g2.setFont(FONT_SMALL);
  for (int i = 0; i < 7; i++) {
    int ww = textWidthL(WD[i]);
    drawTextL(x0 + i * cellW + (cellW - ww) / 2, y0, WD[i]);
  }

  int gx0 = x0, gy0 = y0 + labelH + 2;
  drawRectL(gx0, gy0, gx0 + calW - 1, gy0 + cellH * 6 - 1, COL_FG());

  int days = daysInMonth(1900 + t.tm_year, t.tm_mon);
  int firstW = wdayOfFirst(1900 + t.tm_year, t.tm_mon);

  u8g2.setFont(FONT_SMALL);
  int d = 1;
  for (int row = 0; row < 6 && d <= days; ++row) {
    for (int col = 0; col < 7 && d <= days; ++col) {
      int idx = row * 7 + col;
      if (idx < firstW) continue;
      int cx = gx0 + col * cellW, cy = gy0 + row * cellH;
      drawRectL(cx, cy, cx + cellW - 1, cy + cellH - 1, COL_FG());
      char buf[4];
      snprintf(buf, sizeof(buf), "%d", d);
      int ww = textWidthL(buf);
      int bx = cx + (cellW - ww) / 2, by = cy + cellH - 4;

      // Highlight "today" with a neat circle
      if (d == t.tm_mday) {
        int centerX = cx + cellW / 2;
        int centerY = cy + cellH / 2;
        int r = max(6, min(cellW, cellH) / 2 - 3);
        drawCircleL(centerX, centerY, r, COL_FG());
      }

      drawTextL(bx, by, buf);
      ++d;
    }
  }
}

// =============== QR draw (BLACK only) =========================
static void drawQR_L(int x, int y, int scale) {
  for (int r = 0; r < QR_URL_H; ++r) {
    for (int c = 0; c < QR_URL_W; ++c) {
      int byteIndex = r * QR_URL_ROWBYTES + (c >> 3);
      uint8_t b = pgm_read_byte(&QR_URL[byteIndex]);
      if ((b >> (7 - (c & 7))) & 1) {
        int x1 = x + c * scale, y1 = y + r * scale;
        fillRectL(x1, y1, x1 + scale - 1, y1 + scale - 1, COL_FG());
      }
    }
  }
}

// =============== Page draw (no white fills) ===================
static void drawPage(bool showSeconds) {
  display.clearDisplay();  // single white clear per refresh

  u8_begin_landscape();
  struct tm t;
  getLocalTm(t);

  // Column widths
  const int colGap = 10;
  const int rightMin = 160;  // tuned for helvB24 time
  int calW_land = (LAND_W - 2 * MARGIN - colGap - rightMin);
  calW_land = max(210, (calW_land / 7) * 7);
  int rightW = LAND_W - 2 * MARGIN - colGap - calW_land;
  int rightX = MARGIN + calW_land + colGap;

  // QR scale (fits remaining space; capped at 2x so ~<=98px)
  int availVH = LAND_H - HEADER_BOTTOM - (URL_BLOCK_H + 6) - MARGIN * 2;
  if (availVH < 90) availVH = 90;
  int autoScale = max(2, min(rightW / QR_URL_W, availVH / QR_URL_H));
  int scale = min(autoScale, 2);
  int qrPix = QR_URL_W * scale;

  // Header
  (void)drawHeaderSplit_L(t, rightX, rightW, qrPix, showSeconds);

  // Calendar (LEFT)
  int calY = HEADER_BOTTOM + MARGIN - 20;
  drawCalendar_L(t, MARGIN, calY, calW_land, availVH);

  // QR (RIGHT)
  int qrX = rightX + (rightW - qrPix) / 2;
  int qrY = calY + (availVH - qrPix) / 2;
  if (qrY < calY) qrY = calY;
  //drawQR_L(qrX, qrY, scale);

  // URL bottom
  int urlTop = LAND_H - MARGIN - URL_BLOCK_H + 9;
  drawWrappedUrlCentered_L(urlTop, URL_LINE_SP, LAND_W - 2 * MARGIN);

  display.display();
}

// ============================== SETUP / LOOP ==============================
void setup() {
  Serial.begin(115200);
  delay(50);

  SPI.begin(PIN_SCLK, -1, PIN_SDIN, PIN_CS);

  display.initialize();
  display.High_Power_Mode();
  display.display_on(true);
  display.display_Inversion(PANEL_INVERT ? true : false);  // black-on-white
  display.clearDisplay();
  display.display();

  // First bring up WiFi for initial geo/time/weather
  wifiOn();
  wifiWait();

  if (REGION_MODE == 1 || REGION_MODE == 2) {
    const RegionPreset* g = (REGION_MODE == 1) ? &PRESET_SG : &PRESET_KR;
    gPreset = g;
    g_lat = g->lat; g_lon = g->lon; g_tzIANA = g->tzIANA; g_tzOffsetSec = g->tzOffset;
    syncTimeViaNTP(g->ntpServer);
    fetchWeatherAndSun();
  } else {
    if (WiFi.status() == WL_CONNECTED) fetchIPGeo(g_lat, g_lon, g_tzIANA);
    if (WiFi.status() == WL_CONNECTED) fetchWorldTime(g_tzOffsetSec);
    if (WiFi.status() == WL_CONNECTED) fetchWeatherAndSun();
  }

  // If in power-saving, turn WiFi OFF after initial sync
  if (powerSavingMode) wifiOff();

  // Draw once: seconds only when NOT power-saving
  drawPage(!powerSavingMode);
}

void loop() {
  unsigned long nowMs = millis();
  struct tm t;
  bool got = getLocalTm(t);

  if (!powerSavingMode) {
    // PERFORMANCE MODE (seconds, 1s refresh, WiFi stays ON)
    if (WiFi.status() != WL_CONNECTED) { wifiOn(); wifiWait(); }

    // hourly weather/time refresh while staying online
    if (nowMs - g_lastWeatherMs >= WEATHER_INTERVAL_MS && WiFi.status() == WL_CONNECTED) {
      if (REGION_MODE == 1 || REGION_MODE == 2) syncTimeViaNTP(gPreset->ntpServer);
      else fetchWorldTime(g_tzOffsetSec);
      fetchWeatherAndSun();
    }

    // redraw every second (or at day rollover)
    if (got) {
      if (t.tm_sec != lastDrawSec || t.tm_mday != lastDrawDay) {
        lastDrawSec = t.tm_sec; lastDrawDay = t.tm_mday;
        drawPage(true);   // show seconds
      }
    } else {
      // time not ready → try to sync (WiFi already ON)
      if (REGION_MODE == 1 || REGION_MODE == 2) syncTimeViaNTP(gPreset->ntpServer);
      else fetchWorldTime(g_tzOffsetSec);
      drawPage(true);
    }

  } else {
    // POWER-SAVING MODE (no seconds, 1min refresh, WiFi OFF except hourly sync)
    if (nowMs - g_lastWeatherMs >= WEATHER_INTERVAL_MS) {
      wifiOn(); wifiWait();
      if (WiFi.status() == WL_CONNECTED) {
        if (REGION_MODE == 1 || REGION_MODE == 2) syncTimeViaNTP(gPreset->ntpServer);
        else fetchWorldTime(g_tzOffsetSec);
        fetchWeatherAndSun();
      }
      wifiOff();  // turn OFF immediately after the request
    }

    // redraw every minute (or at day rollover)
    if (got) {
      if (t.tm_min != lastDrawMin || t.tm_mday != lastDrawDay) {
        lastDrawMin = t.tm_min; lastDrawDay = t.tm_mday;
        drawPage(false);  // hide seconds
      }
    } else {
      // time not ready → briefly wake, sync, then sleep again
      wifiOn(); wifiWait();
      if (REGION_MODE == 1 || REGION_MODE == 2) syncTimeViaNTP(gPreset->ntpServer);
      else fetchWorldTime(g_tzOffsetSec);
      wifiOff();
      drawPage(false);
    }
  }

  delay(150);
}
