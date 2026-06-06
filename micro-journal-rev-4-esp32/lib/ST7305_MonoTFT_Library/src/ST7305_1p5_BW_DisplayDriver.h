#ifndef ST7305_1P5_BW_DISPLAY_DRIVER_H
#define ST7305_1P5_BW_DISPLAY_DRIVER_H

#include <Arduino.h>
#include <SPI.h>
#include <ST73XX_UI.h>
#include <ST73xxPins.h> // 必须包含这个文件才能使用 ST73xxPins 结构体

#define ST7305_COLOR_WHITE (0x00)
#define ST7305_COLOR_BLACK (0x01)

// 类名必须是 1p5
class ST7305_1p5_BW_DisplayDriver : public ST73XX_UI {
public:
    // 仅保留使用 ST73xxPins 结构体的构造函数 (与 2.9 驱动风格一致)
    ST7305_1p5_BW_DisplayDriver(const ST73xxPins& pins, SPIClass& spi);
    
    // 移除默认引脚构造函数 (与 2.9 驱动风格一致)
    // ST7305_1p5_BW_DisplayDriver(SPIClass& spi); // 移除

    ~ST7305_1p5_BW_DisplayDriver();

    void initialize();
    void fill(uint8_t data);

    void clearDisplay();

    void writePoint(uint x, uint y, bool enabled) override; 
    void writePoint(uint x, uint y, uint16_t data) override;

    void display();

    void Initial_ST7305();
    
    // 从 2.9 驱动中添加的控制方法 (即使 1.5 驱动未使用，也保留接口一致)
    void Low_Power_Mode();
    void High_Power_Mode();
    void display_on(bool enabled);
    void display_Inversion(bool enabled);

private:
    void address(); 
    void Write_Register(uint8_t idat);
    void Write_Parameter(uint8_t ddat);
    
    // 显存缓冲区
    uint8_t* display_buffer;

    // 成员变量（与 2.9 驱动风格一致）
    const int DC_PIN;
    const int RES_PIN;
    const int CS_PIN;
    const int SCLK_PIN;
    const int SDIN_PIN;
    
    // 尺寸信息 (直接在 .cpp 构造函数中初始化)
    const int LCD_WIDTH;
    const int LCD_HIGH;
    const int LCD_DATA_WIDTH;
    const int LCD_DATA_HIGH;
    const int DISPLAY_BUFFER_LENGTH;
    
    SPIClass& spiRef; 
};

#endif // ST7305_1P5_BW_DISPLAY_DRIVER_H