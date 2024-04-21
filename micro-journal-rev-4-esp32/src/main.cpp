#include "app/app.h"
#include "service/service.h"
#include "service/keyboard/keyboard.h"

#define BAUD_RATE 9600

// Dual Core Support
TaskHandle_t Task0;
TaskHandle_t Task1;

void Core0(void *parameter)
{
  for (;;)
  {    
    //service_loop();
  }
}

void Core1(void *parameter)
{
  for (;;)
  {
    //app_loop();
  }
}

// runs once when the board is up
void setup()
{
  // Setup serial for ESP32
  Serial.begin(BAUD_RATE);
  while (!Serial)
    ; // Wait for the serial port to connect
  app_log("\nDevice Started. Baud rate: %d\n", BAUD_RATE);

  // app setup
  app_setup();

  // service setup
  service_setup();

}

// Main loop is ignored as the tasks are separated per core
void loop()
{
  keyboard_loop();
}
