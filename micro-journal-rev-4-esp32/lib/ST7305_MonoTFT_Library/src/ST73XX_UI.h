#ifndef ST73XX_UI_H
#define ST73XX_UI_H

#include <Arduino.h>
#include <SPI.h>

class ST73XX_UI {
public:
    ST73XX_UI(int16_t w, int16_t h);
    ~ST73XX_UI();

    virtual void writePoint(uint x, uint y, bool enabled);
    virtual void writePoint(uint x, uint y, uint16_t color);

    void drawFastHLine(int16_t x, int16_t y, int16_t len, uint16_t color);
    void drawFastVLine(int16_t x, int16_t y, int16_t len, uint16_t color);

    // 画直线
    void drawLine(uint x1, uint y1, uint x2, uint y2, uint16_t color);

    // 画三角形
    void drawTriangle(uint x1, uint y1, uint x2, uint y2, uint x3, uint y3, uint16_t color);

    // 画实心三角形
    void drawFilledTriangle(int x0, int y0, int x1, int y1, int x2, int y2, uint16_t color);

    // 画矩形
    void drawRectangle(uint x1, uint y1, uint x2, uint y2, uint16_t color);

    // 画实心矩形
    void drawFilledRectangle(uint x1, uint y1, uint x2, uint y2, uint16_t color);

    // 画圆形
    void drawCircle(uint xc, uint yc, uint r, uint16_t color);

    // 画实心圆形
    void drawFilledCircle(int centerX, int centerY, int radius, uint16_t color);

    // 画多边形
    void drawPolygon(uint* points, int n, uint16_t color);

    // 画实心多边形
    void drawFilledPolygon(uint* points, int n, uint16_t color);

private:
    const int WIDTH;
    const int HEIGHT;
};

#endif
