#include "Menu.h"
#include "app/app.h"
#include "display/display.h"

// sub menu module
#include "Home/Home.h"
#include "Storage/Storage.h"

// state
bool menu_clear = false;

// previously selected menu
int menu_state_prev = -1;

//
void Menu_setup(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    //
    JsonDocument &app = status();

    // refresh the background
    menu_clear = true;
    menu_state_prev = -1;
}

void Menu_render(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    JsonDocument &app = status();

    // check if app clear is set as well
    if (app["clear"].as<bool>() == true)
    {
        // reset global clear flag
        app["clear"] = false;

        // clear screen
        menu_clear = true;
    }

    if (!menu_clear)
    {
        // nothing is changed
        // skip this rendering cycle
        return;
    }

    // clear screen
    {
        //
        ptft->fillScreen(TFT_BLACK);

        //
        menu_clear = false;
    }

    // draw sub module of menu
    int menu_state = app["menu"]["state"].as<int>();

    if (menu_state == MENU_HOME)
    {
        if (menu_state_prev != menu_state)
            Home_setup(ptft, pu8f);

        Home_render(ptft, pu8f);
    }

    else if (menu_state == MENU_STORAGE)
    {
        if (menu_state_prev != menu_state)
            Storage_setup(ptft, pu8f);

        Storage_render(ptft, pu8f);
    }

    // save prev state
    menu_state_prev = menu_state;
}

//
void Menu_keyboard(char key, bool pressed)
{
    if (!pressed)
        return;

    //
    JsonDocument &app = status();

    // clear background for every key stroke
    Menu_clear();

    // based on the current menu state
    int menu_state = app["menu"]["state"].as<int>();
    if (menu_state == MENU_HOME)
    {
        Home_keyboard(key, pressed);
        return;
    }
    else if(menu_state == MENU_STORAGE)
    {
        Storage_keyboard(key, pressed);
        return;
    }
}

//
void Menu_clear()
{
    menu_clear = true;
}
