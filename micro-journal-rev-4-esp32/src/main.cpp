#include "app/app.h"
#include "SD/sd.h"
#include "config/config.h"
#include "display/display.h"
#include "keyboard/keyboard.h"

#define BAUD_RATE 9600

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
}

// Main loop is ignored as the tasks are separated per core
void loop()
{
  //
  display_loop();

  //
  keyboard_loop();
}
