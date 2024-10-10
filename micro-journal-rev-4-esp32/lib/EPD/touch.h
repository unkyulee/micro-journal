#pragma once

#ifndef ARDUINO
#error touch.h requires the Arduino framework
#endif

#include <Arduino.h>
#include <Wire.h>

#define TOUCH_SLAVE_ADDRESS   0x5A

class TouchClass
{

    typedef struct {
        uint8_t id;
        uint8_t state;
        uint16_t x;
        uint16_t y;
    } TouchData_t;

public:
    bool    begin(TwoWire &port = Wire, uint8_t addr = TOUCH_SLAVE_ADDRESS);

    uint8_t scanPoint();
    void    getPoint(uint16_t &x, uint16_t &y, uint8_t index);
    void    sleep(void);
    void    wakeup(void);

    TouchData_t data[5];

private:
    void    clearFlags(void);


    void    readBytes(uint8_t *data, uint8_t nbytes);

    uint8_t _address;
    bool initialization = false;

    TwoWire *_i2cPort;
};







