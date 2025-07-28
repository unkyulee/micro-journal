#include "button.h"
#include "app/app.h"
#include "display/display.h"
#include "keyboard/keyboard.h"

//
#include <Button2.h>

//
#define MENU_PIN 15
#define BACK_PIN 16

//
Button2 buttonMenu;
Button2 buttonBack;

void handleTapMenu(Button2 &b)
{
    // open up the menu
    display_keyboard(MENU, true, 69);
    display_keyboard(MENU, false, 69);
}

void handleTapBack(Button2 &b)
{
    JsonDocument &app = status();
    int screen = app["screen"].as<int>();

    if (screen == MENUSCREEN)
    {
        //
        display_keyboard('b', false); // send B when menu
    }

    else
    {
        // go to sleep
        _log("Go to sleep\n");
        app["screen"] = SLEEPSCREEN;
    }
}

//
void button_setup()
{
    // setup two buttons on the device
    buttonMenu.begin(MENU_PIN);
    buttonMenu.setTapHandler(handleTapMenu);

    buttonBack.begin(BACK_PIN);
    buttonBack.setTapHandler(handleTapBack);
}

///
void button_loop()
{
    buttonMenu.loop();
    buttonBack.loop();
}