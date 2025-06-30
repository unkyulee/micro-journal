#include "Knob.h"
#include "display/display.h"

#include <RotaryEncoder.h>

RotaryEncoder *encoder = nullptr;

// This interrupt routine will be called on any change of one of the input signals
void checkPosition()
{
    encoder->tick(); // just call tick() to check the state.
}

void knob_setup()
{
    encoder = new RotaryEncoder(22, 26, RotaryEncoder::LatchMode::TWO03);
    // register interrupt routine
    attachInterrupt(digitalPinToInterrupt(22), checkPosition, CHANGE);
    attachInterrupt(digitalPinToInterrupt(26), checkPosition, CHANGE);
}

void knob_loop()
{
    static int lastPos = 0;
    static int lastDir = 0;
    static int dirCount = 0;
    static unsigned long lastMoveTime = 0;

    encoder->tick(); // just call tick() to check the state.

    int newPos = encoder->getPosition();
    int newDir = (int)encoder->getDirection();

    if (newPos != lastPos)
    {
        unsigned long now = millis();

        if (newDir == lastDir && newDir != 0)
        {
            dirCount++;
            if (dirCount == 2)
            {
                Serial.print("Valid rotation detected! Direction: ");
                Serial.println(newDir);
                dirCount = 0; // Reset after valid rotation

                // newDir - 1 UP
                // newDir - -1 DOWN
                if (newDir == 1)
                {
                    display_keyboard(20, true);
                }
                else if (newDir == -1)
                {
                    display_keyboard(21, true);
                }
            }
        }
        else
        {
            dirCount = 1; // Start new count for new direction
        }

        lastDir = newDir;
        lastMoveTime = now;
        lastPos = newPos;
    }

    // Reset if no action within 1 second
    if (dirCount > 0 && (millis() - lastMoveTime > 1000))
    {
        dirCount = 0;
        lastDir = 0;
    }
}