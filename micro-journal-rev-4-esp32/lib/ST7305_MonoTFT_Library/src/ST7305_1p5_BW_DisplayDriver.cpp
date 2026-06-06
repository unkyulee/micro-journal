#include "ST7305_1p5_BW_DisplayDriver.h"
#include <stdlib.h> 

// 200x200 的硬编码尺寸参数
#define LCD_1P5_WIDTH           200
#define LCD_1P5_HIGH            200
#define LCD_1P5_DATA_WIDTH      51
#define LCD_1P5_DATA_HIGH       100
#define LCD_1P5_BUFFER_LENGTH   (LCD_1P5_DATA_WIDTH * LCD_1P5_DATA_HIGH) // 5100 bytes

// =================================================================
// 构造函数和析构函数
// =================================================================

// 构造函数: 使用 pins 结构体 (假设 pins.rst 是正确的成员名以解决编译错误)
ST7305_1p5_BW_DisplayDriver::ST7305_1p5_BW_DisplayDriver(const ST73xxPins& pins, SPIClass& spi) :
    // 核心修正: 使用 pins.rst 而不是 pins.res
    DC_PIN(pins.dc), RES_PIN(pins.rst), CS_PIN(pins.cs), SCLK_PIN(pins.sclk), SDIN_PIN(pins.sdin),
    // 尺寸硬编码
    LCD_WIDTH(LCD_1P5_WIDTH), LCD_HIGH(LCD_1P5_HIGH),
    ST73XX_UI(LCD_1P5_WIDTH, LCD_1P5_HIGH),
    LCD_DATA_WIDTH(LCD_1P5_DATA_WIDTH), LCD_DATA_HIGH(LCD_1P5_DATA_HIGH), DISPLAY_BUFFER_LENGTH(LCD_1P5_BUFFER_LENGTH),
    spiRef(spi)
{
    display_buffer = new uint8_t[DISPLAY_BUFFER_LENGTH];
    if (display_buffer) {
        memset(display_buffer, 0x00, DISPLAY_BUFFER_LENGTH);
    }
}

// 析构函数
ST7305_1p5_BW_DisplayDriver::~ST7305_1p5_BW_DisplayDriver() {
    delete[] display_buffer;
}

// =================================================================
// 硬件和底层通信 (与 2.9 驱动风格一致)
// =================================================================

void ST7305_1p5_BW_DisplayDriver::Write_Register(uint8_t idat) {
    digitalWrite(DC_PIN, LOW); 
    digitalWrite(CS_PIN, LOW);
    spiRef.write(idat); 
    digitalWrite(CS_PIN, HIGH);
}

void ST7305_1p5_BW_DisplayDriver::Write_Parameter(uint8_t ddat) {
    digitalWrite(DC_PIN, HIGH); 
    digitalWrite(CS_PIN, LOW);
    spiRef.write(ddat); 
    digitalWrite(CS_PIN, HIGH);
}

// =================================================================
// 核心公共方法 (与 2.9 驱动保持风格，但逻辑使用 1.5 的)
// =================================================================

void ST7305_1p5_BW_DisplayDriver::initialize() {
    pinMode(DC_PIN, OUTPUT);
    pinMode(RES_PIN, OUTPUT);
    pinMode(CS_PIN, OUTPUT);
    
    spiRef.begin();
    spiRef.setBitOrder(MSBFIRST);
    spiRef.setDataMode(SPI_MODE0);
    spiRef.setClockDivider(SPI_CLOCK_DIV2); 

    Initial_ST7305();
}

void ST7305_1p5_BW_DisplayDriver::fill(uint8_t data) {
    if (display_buffer) {
        memset(display_buffer, data, DISPLAY_BUFFER_LENGTH);
    }
}

void ST7305_1p5_BW_DisplayDriver::clearDisplay() {
    fill(0x00); 
}

void ST7305_1p5_BW_DisplayDriver::writePoint(uint x, uint y, bool enabled) {
    if (x >= LCD_WIDTH || y >= LCD_HIGH) return;

    // 核心像素打包逻辑 (2x4 垂直交错，未更改)
    x += 4; 
    uint real_x = x / 4; 
    uint real_y = y / 2; 
    uint write_byte_index = real_y * LCD_DATA_WIDTH + real_x;
    
    uint one_two = (y % 2 == 0) ? 0 : 1; 
    uint line_bit_4 = x % 4; 
    uint8_t write_bit = 7 - (line_bit_4 * 2 + one_two); 

    if (write_byte_index >= DISPLAY_BUFFER_LENGTH) return;
    
    if (enabled) {
        display_buffer[write_byte_index] |= (1 << write_bit);
    } else {
        display_buffer[write_byte_index] &= ~(1 << write_bit);
    }
}

void ST7305_1p5_BW_DisplayDriver::writePoint(uint x, uint y, uint16_t data) {
    writePoint(x, y, data != 0); 
}

void ST7305_1p5_BW_DisplayDriver::display() {
    address(); 
    
    digitalWrite(DC_PIN, HIGH);
    digitalWrite(CS_PIN, LOW);
    
    // 批量发送数据
    spiRef.writeBytes(display_buffer, DISPLAY_BUFFER_LENGTH); 
    
    digitalWrite(CS_PIN, HIGH);
}

// =================================================================
// 电源和模式控制方法 (从 2.9 驱动添加，保持接口一致)
// =================================================================

void ST7305_1p5_BW_DisplayDriver::display_on(bool enabled){
    if(enabled){
        Write_Register(0x29); //DISPLAY ON  
    }else{
        Write_Register(0x28); //DISPLAY OFF  
    }
}

void ST7305_1p5_BW_DisplayDriver::display_Inversion(bool enabled){
    if(enabled){
        Write_Register(0x21); //Display Inversion On 
    }else{
        Write_Register(0x20); //Display Inversion Off 
    }
}

void ST7305_1p5_BW_DisplayDriver::Low_Power_Mode() {
    Write_Register(0x10); // Sleep In
    delay(100);
}

void ST7305_1p5_BW_DisplayDriver::High_Power_Mode() {
    Write_Register(0x11); // Sleep Out
    delay(120);
}

// =================================================================
// 驱动芯片初始化序列 (基于您的 1p54.cpp 文件，未更改)
// =================================================================

void ST7305_1p5_BW_DisplayDriver::Initial_ST7305() {
    digitalWrite(RES_PIN, HIGH);	
    delay(10);
    digitalWrite(RES_PIN, LOW);
    delay(10);	
    digitalWrite(RES_PIN, HIGH);	
    delay(10);

    Write_Register(0xD6); Write_Parameter(0X17); Write_Parameter(0X02);
    Write_Register(0xD1); Write_Parameter(0X01);
    
    Write_Register(0xC0); Write_Parameter(0X12); Write_Parameter(0X0a);
    Write_Register(0xC1); Write_Parameter(115); Write_Parameter(0X3E); Write_Parameter(0X3C); Write_Parameter(0X3C);
    Write_Register(0xC2); Write_Parameter(0); Write_Parameter(0X21); Write_Parameter(0X23); Write_Parameter(0X23);
    Write_Register(0xC4); Write_Parameter(50); Write_Parameter(0X5C); Write_Parameter(0X5A); Write_Parameter(0X5A);
    Write_Register(0xC5); Write_Parameter(50); Write_Parameter(0X35); Write_Parameter(0X37); Write_Parameter(0X37);
    
    Write_Register(0xD8); Write_Parameter(0X80); Write_Parameter(0XE9);
    Write_Register(0xB2); Write_Parameter(0X12);
    
    Write_Register(0xB3); Write_Parameter(0XE5); Write_Parameter(0XF6); Write_Parameter(0X17); Write_Parameter(0X77); 
    Write_Parameter(0X77); Write_Parameter(0X77); Write_Parameter(0X77); Write_Parameter(0X77); Write_Parameter(0X77); Write_Parameter(0X71);
    
    Write_Register(0xB4); Write_Parameter(0X05); Write_Parameter(0X46); Write_Parameter(0X77); Write_Parameter(0X77); 
    Write_Parameter(0X77); Write_Parameter(0X77); Write_Parameter(0X76); Write_Parameter(0X45);
    
    Write_Register(0x62); Write_Parameter(0X32); Write_Parameter(0X03); Write_Parameter(0X1F);
    Write_Register(0xB7); Write_Parameter(0X13);
    Write_Register(0xB0); Write_Parameter(0X32); 

    Write_Register(0x11); delay(120); 

    Write_Register(0xC9); Write_Parameter(0X00);
    
    Write_Register(0x36); Write_Parameter(0X48); 
    Write_Register(0x3A); Write_Parameter(0X11); 
    Write_Register(0xB9); Write_Parameter(0X20); 
    Write_Register(0xB8); Write_Parameter(0x29);

    Write_Register(0x2A); Write_Parameter(0X16); Write_Parameter(0X26);
    Write_Register(0x2B); Write_Parameter(0X00); Write_Parameter(0X63);

    Write_Register(0x35); Write_Parameter(0X00); 
    Write_Register(0xD0); Write_Parameter(0XFF); 

    Write_Register(0x38); 
    Write_Register(0x29); 
    Write_Register(0x20); 

    Write_Register(0xBB); Write_Parameter(0x4F);
}

void ST7305_1p5_BW_DisplayDriver::address() {
    Write_Register(0x2A); Write_Parameter(0X16); Write_Parameter(0X26); 
    Write_Register(0x2B); Write_Parameter(0X00); Write_Parameter(0X63);
    Write_Register(0x2C); 
}