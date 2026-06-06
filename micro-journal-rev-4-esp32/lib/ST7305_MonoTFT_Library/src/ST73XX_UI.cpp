#include <ST73XX_UI.h>
#include <stdlib.h>

#define ABS_DIFF(x, y) (((x) > (y))? ((x) - (y)) : ((y) - (x)))

ST73XX_UI::ST73XX_UI(int16_t w, int16_t h) : WIDTH(w), HEIGHT(h)
{
}

ST73XX_UI::~ST73XX_UI() {
}

void ST73XX_UI::writePoint(uint x, uint y, bool enabled) {
    
}

void ST73XX_UI::writePoint(uint x, uint y, uint16_t color) {
    
}

// 函数功能：

// 这个函数名为u8g2_draw_hv_line，它的作用是在使用u8g2库的图形上下文中绘制水平或垂直的线段。

// 参数解释：

//     u8g2_font_t *u8g2：这是一个指向u8g2库的字体结构的指针，通过这个指针可以访问到u8g2库的图形绘制函数。
//     int16_t x和int16_t y：表示线段起点的坐标。
//     int16_t len：线段的长度。
//     uint8_t dir：指定线段的方向，有以下几种取值：
//         0：表示绘制水平线段，从左到右，起点坐标为(x, y)，长度为len。
//         1：表示绘制垂直线段，从上到下，起点坐标为(x, y)，长度为len。
//         2：表示绘制水平线段，从右到左，起点坐标为(x - len + 1, y)，长度为len。
//         3：表示绘制垂直线段，从下到上，起点坐标为(x, y - len + 1)，长度为len。
//     uint16_t color：指定线段的颜色。


// 函数执行过程：

//     根据传入的dir参数的值，选择不同的绘制方式。
//         如果dir为0，调用u8g2->gfx->drawFastHLine(x,y,len,color)绘制从左到右的水平线段。
//         如果dir为1，调用u8g2->gfx->drawFastVLine(x,y,len,color)绘制从上到下的垂直线段。
//         如果dir为2，调用u8g2->gfx->drawFastHLine(x - len + 1,y,len,color)绘制从右到左的水平线段。
//         如果dir为3，调用u8g2->gfx->drawFastVLine(x,y - len + 1,len,color)绘制从下到上的垂直线段。


void ST73XX_UI::drawFastHLine(int16_t x, int16_t y, int16_t len, uint16_t color) {
    // 这里是绘制水平线段的逻辑实现
    // 可以根据具体的图形绘制需求进行实现，例如循环绘制一系列像素点来组成水平线段
    for (int i = x; i < x + len; i++) {
        // 设置坐标 (i, y) 的像素颜色为 color
        // 根据你的实际图形绘制方式进行设置
        writePoint(i, y, color);
    }
}

void ST73XX_UI::drawFastVLine(int16_t x, int16_t y, int16_t len, uint16_t color) {
    // 这里是绘制垂直线段的逻辑实现
    // 可以根据具体的图形绘制需求进行实现，例如循环绘制一系列像素点来组成垂直线段
    for (int i = y; i < y + len; i++) {
        // 设置坐标 (x, i) 的像素颜色为 color
        // 根据你的实际图形绘制方式进行设置
        writePoint(x, i, color);
    }
}

// 画直线
void ST73XX_UI::drawLine(uint x1, uint y1, uint x2, uint y2, uint16_t color) {
    int dx = ABS_DIFF(x2, x1);
    int dy = ABS_DIFF(y2, y1);
    int sx = (x1 < x2)? 1 : -1;
    int sy = (y1 < y2)? 1 : -1;
    int err = dx - dy;

    while (true) {
        writePoint(x1, y1, color);
        if (x1 == x2 && y1 == y2) break;
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y1 += sy;
        }
    }
}

// 画三角形
void ST73XX_UI::drawTriangle(uint x1, uint y1, uint x2, uint y2, uint x3, uint y3, uint16_t color) {
    drawLine(x1, y1, x2, y2, color);
    drawLine(x2, y2, x3, y3, color);
    drawLine(x3, y3, x1, y1, color);
}

// 绘制实心三角形
void ST73XX_UI::drawFilledTriangle(int x0, int y0, int x1, int y1, int x2, int y2, uint16_t color) {
    uint polygonPoints[] = {x0, y0, x1, y1, x2, y2};
    drawFilledPolygon(polygonPoints, 3, color);
}

// 画矩形
void ST73XX_UI::drawRectangle(uint x1, uint y1, uint x2, uint y2, uint16_t color) {
    drawLine(x1, y1, x2, y1, color);
    drawLine(x2, y1, x2, y2, color);
    drawLine(x2, y2, x1, y2, color);
    drawLine(x1, y2, x1, y1, color);
}

// 画实心矩形
void ST73XX_UI::drawFilledRectangle(uint x1, uint y1, uint x2, uint y2, uint16_t color) {
    for (uint y = y1; y <= y2; y++) {
        for (uint x = x1; x <= x2; x++) {
            writePoint(x, y, color);
        }
    }
}

// 画圆形（简单的 Bresenham 算法）
void ST73XX_UI::drawCircle(uint xc, uint yc, uint r, uint16_t color) {
    int x = 0;
    int y = r;
    int d = 3 - 2 * r;
    while (x <= y) {
        writePoint(xc + x, yc + y, color);
        writePoint(xc - x, yc + y, color);
        writePoint(xc + x, yc - y, color);
        writePoint(xc - x, yc - y, color);
        writePoint(xc + y, yc + x, color);
        writePoint(xc - y, yc + x, color);
        writePoint(xc + y, yc - x, color);
        writePoint(xc - y, yc - x, color);
        if (d < 0) {
            d = d + 4 * x + 6;
        } else {
            d = d + 4 * (x - y) + 10;
            y--;
        }
        x++;
    }
}

// 绘制实心圆形并调用 writePoint 函数
void ST73XX_UI::drawFilledCircle(int centerX, int centerY, int radius, uint16_t color) {
    int x = radius;
    int y = 0;
    int decisionOver2 = 1 - x;

    while (x >= y) {
        // 绘制水平线段填充圆形的八分之一
        for (int i = centerX - x; i <= centerX + x; i++) {
            writePoint(i, centerY + y, color);
            writePoint(i, centerY - y, color);
        }
        for (int i = centerX - y; i <= centerX + y; i++) {
            writePoint(i, centerY + x, color);
            writePoint(i, centerY - x, color);
        }

        y++;
        if (decisionOver2 <= 0) {
            decisionOver2 += 2 * y + 1;
        } else {
            x--;
            decisionOver2 += 2 * (y - x) + 1;
        }
    }
}

// 画多边形（假设顶点数组为 points，n 为顶点数量）
void ST73XX_UI::drawPolygon(uint* points, int n, uint16_t color) {
    for (int i = 0; i < n; i++) {
        int j = (i + 1) % n;
        drawLine(points[2 * i], points[2 * i + 1], points[2 * j], points[2 * j + 1], color);
    }
}

// 画实心多边形（扫描线算法，假设顶点数组为 points，n 为顶点数量）
void ST73XX_UI::drawFilledPolygon(uint* points, int n, uint16_t color) {
    // 找到多边形的最小和最大 y 值
    int minY = points[1];
    int maxY = points[1];
    for (int i = 0; i < n; ++i) {
        int y = points[2 * i + 1];
        if (y < minY) minY = y;
        if (y > maxY) maxY = y;
    }

    int* intersections = new int[2 * n];
    int intersectionCount = 0;

    for (int y = minY; y <= maxY; ++y) {
        intersectionCount = 0;
        for (int i = 0; i < n; ++i) {
            int nextIndex = (i + 1) % n;
            int x1 = points[2 * i];
            int y1 = points[2 * i + 1];
            int x2 = points[2 * nextIndex];
            int y2 = points[2 * nextIndex + 1];
            if ((y1 <= y && y2 > y) || (y2 <= y && y1 > y)) {
                int x = x1 + (y - y1) * (x2 - x1) / (y2 - y1);
                intersections[intersectionCount++] = x;
            }
        }

        // 排序交点
        for (int i = 0; i < intersectionCount - 1; ++i) {
            for (int j = i + 1; j < intersectionCount; ++j) {
                if (intersections[i] > intersections[j]) {
                    int temp = intersections[i];
                    intersections[i] = intersections[j];
                    intersections[j] = temp;
                }
            }
        }

        for (int i = 0; i < intersectionCount; i += 2) {
            int startX = intersections[i];
            int endX = intersections[i + 1];
            for (int x = startX; x < endX; ++x) {
                writePoint(x, y, color);
            }
        }
    }

    delete[] intersections;
}
