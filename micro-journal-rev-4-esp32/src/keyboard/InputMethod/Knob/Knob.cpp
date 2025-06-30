#include "Knob.h"

#include <a21.hpp>
using namespace a21;

EC11 encoder;
const int encoderPinA = 22;
const int encoderPinB = 26;

void pinDidChange()
{
    encoder.checkPins(digitalRead(encoderPinA), digitalRead(encoderPinB));
}

void knob_setup()
{
    attachInterrupt(digitalPinToInterrupt(encoderPinA), pinDidChange, CHANGE);
    attachInterrupt(digitalPinToInterrupt(encoderPinB), pinDidChange, CHANGE);
}

static int value = 0;

void knob_loop()
{
    EC11Event e;
    if (encoder.read(&e))
    {

        // OK, got an event waiting to be handled, its count field is number of steps since the last check.

        if (e.type == EC11Event::StepCW)
        {
            // Clock-wise.
            value += e.count;
        }
        else
        {
            // Counter clock-wise.
            value -= e.count;
        }

        Serial.println(value);
    }
}