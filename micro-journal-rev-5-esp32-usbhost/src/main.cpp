#include "app/app.h"
#include "service/service.h"

#define BAUD_RATE 9600

// runs once when the board is up
void setup()
{
  // Setup serial for ESP32
  Serial.begin(BAUD_RATE);
  while (!Serial)
    ; // Wait for the serial port to connect

  delay(100);

  //
  app_log("\nDevice Started. Baud rate: %d\n", BAUD_RATE);

  // app setup
  app_setup();

  // service setup
  service_setup();
}

// Main loop is ignored as the tasks are separated per core
void loop()
{
  //
  app_loop();
  service_loop();
}
