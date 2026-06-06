
#ifndef ST7305_BITMAP_HELPER_H
#define ST7305_BITMAP_HELPER_H

#include <Arduino.h>
#include "ST7305_2p9_BW_DisplayDriver.h"

// Draw a 1-bit bitmap to the ST7305 framebuffer.
// bitmap: row-major, MSB-first in each byte (like Adafruit_GFX drawBitmap)
// x,y: top-left on screen
// w,h: bitmap width/height in pixels
// color: ST7305_COLOR_BLACK or ST7305_COLOR_WHITE
static inline void st7305_drawBitmap1bit(ST7305_2p9_BW_DisplayDriver &display,
                                         int16_t x, int16_t y,
                                         const uint8_t *bitmap,
                                         int16_t w, int16_t h,
                                         uint16_t color,
                                         bool drawBackground = false,
                                         uint16_t bg = ST7305_COLOR_WHITE)
{
  int bytesPerRow = (w + 7) / 8;
  for (int16_t yy = 0; yy < h; yy++) {
    const uint8_t *row = bitmap + yy * bytesPerRow;
    uint8_t current = 0;
    for (int16_t xx = 0; xx < w; xx++) {
      if ((xx & 7) == 0) current = row[xx >> 3];
      bool bitSet = (current & (0x80 >> (xx & 7))) != 0;
      int16_t sx = x + xx;
      int16_t sy = y + yy;

      if (bitSet) {
        display.writePoint(sx, sy, color);
      } else if (drawBackground) {
        display.writePoint(sx, sy, bg);
      }
    }
  }
}

#endif // ST7305_BITMAP_HELPER_H
