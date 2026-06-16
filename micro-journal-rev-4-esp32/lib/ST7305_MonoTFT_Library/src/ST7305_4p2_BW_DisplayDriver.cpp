#include <ST73XX_UI.h>
#include <ST7305_4p2_BW_DisplayDriver.h>
#include <stdlib.h>

ST7305_4p2_BW_DisplayDriver::ST7305_4p2_BW_DisplayDriver(
    int dcPin,
    int resPin,
    int csPin,
    int sclkPin,
    int sdinPin,
    SPIClass &spi)
    : DC_PIN(dcPin),
      RES_PIN(resPin),
      CS_PIN(csPin),
      SCLK_PIN(sclkPin),
      SDIN_PIN(sdinPin),
      LCD_WIDTH(300),
      LCD_HEIGHT(400),
      ST73XX_UI(300, 400),
      LCD_DATA_WIDTH(150),
      LCD_DATA_HEIGHT(200),
      DISPLAY_BUFFER_LENGTH(30000),
      spiRef(spi)
{
    display_buffer = new uint8_t[DISPLAY_BUFFER_LENGTH];
}

ST7305_4p2_BW_DisplayDriver::~ST7305_4p2_BW_DisplayDriver()
{
    delete[] display_buffer;
}

void ST7305_4p2_BW_DisplayDriver::initialize()
{
    pinMode(DC_PIN, OUTPUT);
    pinMode(RES_PIN, OUTPUT);
    pinMode(CS_PIN, OUTPUT);

    digitalWrite(RES_PIN, HIGH);

    spiRef.setFrequency(40000000);
    spiRef.begin(SCLK_PIN, -1, SDIN_PIN, -1);

    Initial_ST7305();
    fill(0x00);
}

void ST7305_4p2_BW_DisplayDriver::fill(uint8_t data)
{
    memset(display_buffer, data, DISPLAY_BUFFER_LENGTH);
    Serial.printf("fill data = 0x%x\n", data);
}

void ST7305_4p2_BW_DisplayDriver::clearDisplay()
{
    memset(display_buffer, 0x00, DISPLAY_BUFFER_LENGTH);
}

static constexpr uint8_t ST7306_WHITE = 0x00;
static constexpr uint8_t ST7306_BLACK = 0x0F;

void ST7305_4p2_BW_DisplayDriver::writePhysicalPoint(uint x, uint y, bool black)
{
    if (x >= LCD_HEIGHT || y >= LCD_WIDTH)
        return;

    uint px = LCD_WIDTH - 1 - y;
    uint py = x;

    uint real_x = px / 2;
    uint real_y = py / 2;

    uint write_byte_index = real_y * LCD_DATA_WIDTH + real_x;

    if (write_byte_index >= DISPLAY_BUFFER_LENGTH)
        return;

    uint8_t color = black ? ST7306_BLACK : ST7306_WHITE;

    if ((px & 1) == 0) 
    {
        display_buffer[write_byte_index] =
            (display_buffer[write_byte_index] & 0x0F) | (color << 4);
    }
    else
    {
        display_buffer[write_byte_index] =
            (display_buffer[write_byte_index] & 0xF0) | (color & 0x0F);
    }
}

void ST7305_4p2_BW_DisplayDriver::writePoint(uint x, uint y, bool data)
{
    writePhysicalPoint(x, y, data);
}

void ST7305_4p2_BW_DisplayDriver::writePoint(uint x, uint y, uint16_t data)
{
    writePhysicalPoint(x, y, data != 0);
}

void ST7305_4p2_BW_DisplayDriver::address()
{
    Write_Register(0x2A);
    Write_Parameter(0x05);
    Write_Parameter(0x36);

    Write_Register(0x2B);
    Write_Parameter(0x00);
    Write_Parameter(0xC7);

    Write_Register(0x2C);
}

void ST7305_4p2_BW_DisplayDriver::display()
{
    address();
    digitalWrite(DC_PIN, HIGH);
    digitalWrite(CS_PIN, LOW);
    spiRef.writeBytes(display_buffer, DISPLAY_BUFFER_LENGTH);
    digitalWrite(CS_PIN, HIGH);
}

void ST7305_4p2_BW_DisplayDriver::Initial_ST7305()
{
    digitalWrite(RES_PIN, HIGH);
    delay(10);
    digitalWrite(RES_PIN, LOW);
    delay(10);
    digitalWrite(RES_PIN, HIGH);
    delay(10);

    /////////////HSD 4.2” 300x400 Mono High Scan Rate Initial Code (8Hz)/////////////////
    Write_Register(0xD6); // NVM Load Control
    Write_Parameter(0X17);
    Write_Parameter(0X02);

    Write_Register(0xD1); // Booster Enable
    Write_Parameter(0X01);

    Write_Register(0xC0);  // Gate Voltage Setting
    Write_Parameter(0X11); // VGH 00:8V  04:10V  08:12V   0E:15V  11:16.5V
    Write_Parameter(0X04); // VGL 00:-5V   04:-7V   0A:-10V

    // VLC=3.6V (12/-5)(delta Vp=0.6V)
    Write_Register(0xC1);  // VSHP Setting (4.8V)
    Write_Parameter(0X41); // VSHP1  5.0V
    Write_Parameter(0X41); // VSHP2  5.0V
    Write_Parameter(0X41); // VSHP3  5.0V
    Write_Parameter(0X41); // VSHP4	 5.0V

    Write_Register(0xC2);  // VSLP Setting (0.98V)
    Write_Parameter(0X19); // VSLP1  0.5V
    Write_Parameter(0X19); // VSLP2  0.5V
    Write_Parameter(0X19); // VSLP3  0.5V
    Write_Parameter(0X19); // VSLP4  0.5V

    Write_Register(0xC4);  // VSHN Setting (-3.6V)
    Write_Parameter(0X41); // VSHN1	  3.8V
    Write_Parameter(0X41); // VSHN2 	3.8V
    Write_Parameter(0X41); // VSHN3 	3.8V
    Write_Parameter(0X41); // VSHN4 	3.8V

    Write_Register(0xC5);  // VSLN Setting (0.22V)
    Write_Parameter(0X19); // VSLN1 	0.5V
    Write_Parameter(0X19); // VSLN2 	0.5V
    Write_Parameter(0X19); // VSLN3 	0.5V
    Write_Parameter(0X19); // VSLN4   0.5V

    Write_Register(0xD8);  // HPM=32Hz
    Write_Parameter(0XA6); //~51Hz
    Write_Parameter(0XE9); //~1Hz

    /*-- HPM=32hz ; LPM=> 0x15=8Hz 0x14=4Hz 0x13=2Hz 0x12=1Hz 0x11=0.5Hz 0x10=0.25Hz---*/
    Write_Register(0xB2);  // Frame Rate Control
    Write_Parameter(0X05); // 12--HPM=32hz ; LPM=1hz   05--HPM=16hz ; LPM=8hz

    Write_Register(0xB3); // Update Period Gate EQ Control in HPM
    Write_Parameter(0XE5);
    Write_Parameter(0XF6);
    Write_Parameter(0X05); // HPM EQ Control
    Write_Parameter(0X46);
    Write_Parameter(0X77);
    Write_Parameter(0X77);
    Write_Parameter(0X77);
    Write_Parameter(0X77);
    Write_Parameter(0X76);
    Write_Parameter(0X45);

    Write_Register(0xB4);  // Update Period Gate EQ Control in LPM
    Write_Parameter(0X05); // LPM EQ Control
    Write_Parameter(0X46);
    Write_Parameter(0X77);
    Write_Parameter(0X77);
    Write_Parameter(0X77);
    Write_Parameter(0X77);
    Write_Parameter(0X76);
    Write_Parameter(0X45);

    Write_Register(0x62); // Gate Timing Control
    Write_Parameter(0X32);
    Write_Parameter(0X03);
    Write_Parameter(0X1F);

    Write_Register(0xB7); // Source EQ Enable
    Write_Parameter(0X13);

    Write_Register(0xB0);  // Gate Line Setting
    Write_Parameter(0X64); // 60---384 line    64---400 line

    Write_Register(0x11); // Sleep out
    delay(255);

    Write_Register(0xC9);  // Source Voltage Select
    Write_Parameter(0X00); // VSHP1; VSLP1 ; VSHN1 ; VSLN1

    Write_Register(0x36); // Memory Data Access Control
    // Write_Parameter(0X00); //Memory Data Access Control: MX=0 ; DO=0
    Write_Parameter(0X48); // MX=1 ; DO=1
    // Write_Parameter(0X4c); //MX=1 ; DO=1 GS=1

    Write_Register(0x3A);  // Data Format Select
    Write_Parameter(0X11); // 10:4write for 24bit ; 11: 3write for 24bit

    Write_Register(0xB9);  // Gamma Mode Setting
    Write_Parameter(0X20); // 20: Mono 00:4GS

    Write_Register(0xB8);  // Panel Setting
    Write_Parameter(0X29); // Panel Setting Frame inversion  09:column 29:dot_1-Frame 25:dot_1-Line

    Write_Register(0x21); // Inverse

    // WRITE RAM 300*400
    Write_Register(0x2A);
    Write_Parameter(0X12);
    Write_Parameter(0X2B);

    Write_Register(0x2B);
    Write_Parameter(0X00);
    Write_Parameter(0XC7);

    Write_Register(0x35);
    Write_Parameter(0X00);

    Write_Register(0xD0);
    Write_Parameter(0XFF);

    Write_Register(0x39); // LPM

    Write_Register(0x29); // DISPLAY ON
}

void ST7305_4p2_BW_DisplayDriver::Low_Power_Mode()
{
    Write_Register(0x39); // LPM:Low Power Mode ON
}

void ST7305_4p2_BW_DisplayDriver::High_Power_Mode()
{
    Write_Register(0x38); // HPM:high Power Mode ON
}

void ST7305_4p2_BW_DisplayDriver::display_on(bool enabled)
{
    if (enabled)
    {
        Write_Register(0x29); // DISPLAY ON
    }
    else
    {
        Write_Register(0x28); // DISPLAY OFF
    }
}

void ST7305_4p2_BW_DisplayDriver::display_Inversion(bool enabled)
{
    if (enabled)
    {
        Write_Register(0x21); // Display Inversion On
    }
    else
    {
        Write_Register(0x20); // Display Inversion Off
    }
}

void ST7305_4p2_BW_DisplayDriver::Write_Register(uint8_t idat)
{
    digitalWrite(DC_PIN, LOW);
    digitalWrite(CS_PIN, LOW);
    spiRef.write(idat);
    digitalWrite(CS_PIN, HIGH);
}

void ST7305_4p2_BW_DisplayDriver::Write_Parameter(uint8_t ddat)
{
    digitalWrite(DC_PIN, HIGH);
    digitalWrite(CS_PIN, LOW);
    spiRef.write(ddat);
    digitalWrite(CS_PIN, HIGH);
}

ST7305_4p2_BW_DisplayDriver::ST7305_4p2_BW_DisplayDriver(SPIClass &spi)
    : ST7305_4p2_BW_DisplayDriver(4, 0, 3, 2, 1, spi)
{
    // Uses default pins defined in DisplayConfig.h
}

// Named-pin overload to avoid argument order mistakes
ST7305_4p2_BW_DisplayDriver::ST7305_4p2_BW_DisplayDriver(const ST73xxPins &pins, SPIClass &spi)
    : ST7305_4p2_BW_DisplayDriver(pins.dc, pins.rst, pins.cs, pins.sclk, pins.sdin, spi)
{
}
