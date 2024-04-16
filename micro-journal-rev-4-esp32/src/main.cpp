#include "app/app.h"
#include "service/service.h"

#define BAUD_RATE 9600

// Dual Core Support
TaskHandle_t Task0;
TaskHandle_t Task1;

void Core0(void *parameter)
{
  for (;;)
  {
    
    service_loop();
  }
}

void Core1(void *parameter)
{
  for (;;)
  {
    // code for task 1
    app_loop();
  }
}

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

  xTaskCreatePinnedToCore(
      Core0, /* Function to implement the task */
      "Core0",   /* Name of the task */
      10000,     /* Stack size in words */
      NULL,      /* Task input parameter */
      0,         /* Priority of the task */
      &Core0,    /* Task handle. */
      0);        /* Core where the task should run */

  xTaskCreatePinnedToCore(
      Core1, /* Task function. */
      "Core1",   /* name of task. */
      10000,     /* Stack size of task */
      NULL,      /* parameter of the task */
      1,         /* priority of the task */
      &Core1,    /* Task handle to keep track of created task */
      1);        /* pin task to core 1 */
}

// Main loop is ignored as the tasks are separated per core
void loop()
{
}
