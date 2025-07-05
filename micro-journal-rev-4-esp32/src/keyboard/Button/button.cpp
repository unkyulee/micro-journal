#include "button.h"
#include "app/app.h"
#include "display/display.h"
#include "keyboard/keyboard.h"

#define MENU_PIN 15
#define BACK_PIN 16

//
void button_setup()
{
    // setup two buttons on the device
    pinMode(MENU_PIN, INPUT_PULLUP);
    pinMode(BACK_PIN, INPUT_PULLUP);
}

///
void button_loop()
{
    static unsigned int last = 0;
    if (millis() - last > 1000)
    {
        //
        last = millis();

        JsonDocument &app = status();
        bool disabled = app["config"]["front_panel_button_disabled"].as<bool>();
        if (disabled)
            return;

        // check menu key and back key press
        int menuKey = digitalRead(MENU_PIN);
        int backKey = digitalRead(BACK_PIN);

        // 0 means pressed
        // 1 means released
        if (menuKey == 0)
        {
            // open up the menu
            display_keyboard(MENU);
        }

        if (backKey == 0)
        {
            JsonDocument &app = status();
            int screen = app["screen"].as<int>();

            if (screen == MENUSCREEN)
            {
                //
                display_keyboard('b'); // send back button when menu
            }

            else
            {
                // go to sleep
                _log("Go to sleep\n");
                app["screen"] = SLEEPSCREEN;
            }
        }
    }
}