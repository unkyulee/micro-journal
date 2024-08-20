#include "app/app.h"
#include "SD/sd.h"
#include "config/config.h"
#include "display/display.h"
#include "keyboard/keyboard.h"

#define BAUD_RATE 9600

// Dual Core Support
TaskHandle_t Task0;
void Core0(void *parameter);

// runs once when the board is up
void setup()
{
  // Setup serial for ESP32
  Serial.begin(BAUD_RATE);
  app_log("\nDevice Started. Baud rate: %d\n", BAUD_RATE);

  // SD must be initialized before display
  SD_setup();

  // initialize config
  config_setup();

  // keyboard setup
  keyboard_setup();

  //
  display_setup();

  //
  xTaskCreatePinnedToCore(
      Core0,   // Function to implement the task
      "Core0", // Name of the task
      10000,   // Stack size in words
      NULL,    // Task input parameter
      0,       // Priority of the task
      &Task0,  // Task handle.
      0);      // Core where the task should run
}

// Main loop is ignored as the tasks are separated per core
void loop()
{
  //
  display_loop();

  //
  keyboard_loop();
}

void Core0(void *parameter)
{
  static unsigned int last = 0;
  while (true)
  {
    if (millis() - last > 100)
    {
      //
      last = millis();

      JsonDocument &app = app_status();
      int screen = app["screen"].as<int>();

      // when GIF is playing keyboard gets blocked
      if (screen == SLEEPSCREEN || screen == WAKEUPSCREEN)
      {
        // do the keyboard loop
        keyboard_loop();
      }
    }
  }
}