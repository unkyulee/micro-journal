#include <Arduino.h>

void setup()
{
}

void loop()
{
    Serial.println("Core0: Hello, World!");
    delay(1000);
}

//
void setup1()
{
}

void loop1()
{
    Serial.println("Core1: Hello, World!");
    delay(2000);
}
