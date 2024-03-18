#include "app/app.h"
#include "service/service.h"

// Dual Core
TaskHandle_t TaskMain;
TaskHandle_t TaskSecondary;

#define BAUD_RATE 9600

// Main thread loops will handle the critical tasks for the app
// should not place anything that requries significant blocking tasks
// in order to perform stable and regular operation
void TaskMainLoop(void *parameter)
{
  // app setup
  app_setup_main();

  // service setup
  service_setup_main();

  for (;;)
  {
    //
    app_loop_main();
    service_loop_main();
  }
}

// Code for Core 2 - blocking loops
void TaskSecondaryLoop(void *parameter)
{
  // app setup
  app_setup_secondary();

  // service setup
  service_setup_secondary();

  delay(5000);

  for (;;)
  {
    //
    app_loop_secondary();
    service_loop_secondary();
  }
}

// runs once when the board is up
void setup()
{
  // Setup serial for ESP32
  Serial.begin(BAUD_RATE);
  while (!Serial)
    ; // Wait for the serial port to connect

  //
  app_log("\nDevice Started. Baud rate: %d\n", BAUD_RATE);

  xTaskCreatePinnedToCore(
      TaskMainLoop,         /* Function to implement the task */
      "TaskMainLoop",       /* Name of the task */
      10000,                /* Stack size in words */
      NULL,                 /* Task input parameter */
      0,                    /* Priority of the task */
      &TaskMain,            /* Task handle. */
      0);                   /* Core where the task should run */

  xTaskCreatePinnedToCore(
      TaskSecondaryLoop,    /* Task function. */
      "TaskSecondaryLoop",  /* name of task. */
      10000,                /* Stack size of task */
      NULL,                 /* parameter of the task */
      1,                    /* priority of the task */
      &TaskSecondary,       /* Task handle to keep track of created task */
      1);                   /* pin task to core 1 */
}

// Main loop is ignored as the tasks are separated per core
void loop() {}
