#ifdef ARDUINO

#include "touch.h"

bool TouchClass::begin(TwoWire &port, uint8_t addr)
{
    _i2cPort = &port;
    _address = addr;
    _i2cPort->beginTransmission(_address);
    if (0 == _i2cPort->endTransmission()) {
        wakeup();
        return true;
    }
    return false;
}

void TouchClass::readBytes(uint8_t *data, uint8_t nbytes)
{
    _i2cPort->beginTransmission(_address);  // Initialize the Tx buffer
    _i2cPort->write(data, 2);                // Put data in Tx buffer
    if (0 != _i2cPort->endTransmission()) {
        Serial.println("readBytes error!");
    }
    uint8_t i = 0;
    _i2cPort->requestFrom(_address, nbytes);  // Read bytes from slave register address
    while (_i2cPort->available()) {
        data[i++] = _i2cPort->read();
    }
}

void TouchClass::clearFlags(void)
{
    uint8_t buf[3] = {0xD0, 0X00, 0XAB};
    _i2cPort->beginTransmission(_address);
    _i2cPort->write(buf, 3);
    _i2cPort->endTransmission();
}

uint8_t TouchClass::scanPoint()
{
    uint8_t point = 0;
    uint8_t buffer[40] = {0};

    buffer[0] = 0xD0;
    buffer[1] = 0x00;
    readBytes(buffer, 7);

    if (buffer[0] == 0xAB) {
        clearFlags();
        return 0;
    }

    point = buffer[5] & 0xF;

    if (point == 1) {
        buffer[5] = 0xD0;
        buffer[6] = 0x07;
        readBytes( &buffer[5], 2);

    } else if (point > 1) {
        buffer[5] = 0xD0;
        buffer[6] = 0x07;
        readBytes( &buffer[5], 5 * (point - 1) + 3);
    }
    clearFlags();

    if (point) {
        for (int i = 0; i < point; ++i) {
            data[i].id =  (buffer[i * 5] >> 4) & 0x0F;
            data[i].state = buffer[i * 5] & 0x0F;
            if (data[i].state == 0x06) {
                data[i].state = 0x07;
            } else {
                data[i].state = 0x06;
            }
            data[i].y = (uint16_t)((buffer[i * 5 + 1] << 4) | ((buffer[i * 5 + 3] >> 4) & 0x0F));
            data[i].x = (uint16_t)((buffer[i * 5 + 2] << 4) | (buffer[i * 5 + 3] & 0x0F));
        }
    } else {
        point = 1;
        data[0].id = (buffer[0] >> 4) & 0x0F;
        data[0].state = 0x06;
        data[0].y = (uint16_t)((buffer[0 * 5 + 1] << 4) | ((buffer[0 * 5 + 3] >> 4) & 0x0F));
        data[0].x = (uint16_t)((buffer[0 * 5 + 2] << 4) | (buffer[0 * 5 + 3] & 0x0F));
    }
    // Serial.printf("X:%d Y:%d\n", data[0].x, data[0].y);
    return point;
}


void TouchClass::getPoint(uint16_t &x, uint16_t &y, uint8_t index)
{
    if (index >= 4)return;
    x = data[index].x;
    y = data[index].y;
}


void TouchClass::sleep(void)
{
    uint8_t buf[2] = {0xD1, 0X05};
    _i2cPort->beginTransmission(_address);
    _i2cPort->write(buf, 2);
    _i2cPort->endTransmission();
}

void TouchClass::wakeup(void)
{
    uint8_t buf[2] = {0xD1, 0X06};
    _i2cPort->beginTransmission(_address);
    _i2cPort->write(buf, 2);
    _i2cPort->endTransmission();
}

#endif
