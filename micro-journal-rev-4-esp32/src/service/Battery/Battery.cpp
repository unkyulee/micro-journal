#include "Battery.h"
#include "app/app.h"

//
void battery_setup()
{
    analogReadResolution(12);
    pinMode(BATTERY, INPUT);
}

// rp2040 still works around 2.0 v
const int batteryInterval = 60000;
void battery_loop()
{
    static long lastMeasure = -(batteryInterval - 5000);
    if (millis() - lastMeasure >= batteryInterval)
    {
        lastMeasure = millis();
        int raw = analogRead(BATTERY);

        // Convert raw ADC value to voltage as needed
        float voltage = (raw / 4095.0) * 3.6 * 2; // Example for 12-bit ADC, 3.3V ref

        // app status
        JsonDocument &app = status();
        float percent = (voltage - 2.0) / (4.2 - 2.0) * 100;
        if (percent > 100.0f)
            percent = 100.0f;
        if (percent < 0.0f)
            percent = 0.0f;

        //
        app["battery"] = percent;

        // Log
        _debug("Battery %f %% voltage: %.2f V raw: %d\n", percent, voltage, raw);
    }
}