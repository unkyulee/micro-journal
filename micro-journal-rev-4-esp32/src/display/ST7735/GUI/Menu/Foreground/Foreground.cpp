#include "Foreground.h"
#include "../Menu.h"
#include "app/app.h"
#include "config/config.h"
#include "display/display.h"
#include "../../WordProcessor/WordProcessor.h"

//
void Foreground_setup(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    // when entering the screen
    // clear the screen
    Menu_clear();
}

void Foreground_print_color(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f, uint16_t color)
{
    int cursorX = ptft->getCursorX();
    int cursorY = ptft->getCursorY();
    ptft->fillCircle(cursorX + 8, cursorY + 8, 6, TFT_WHITE);
    ptft->fillCircle(cursorX + 8, cursorY + 8, 5, color);
    ptft->setCursor(cursorX + 25, cursorY);
}

//
void Foreground_render(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    ptft->setCursor(0, 30, 2);
    ptft->setTextSize(1);
    //
    ptft->setTextColor(TFT_WHITE, TFT_BLACK);
    ptft->println(" FOREGROUND COLOR");
    ptft->println("");

    //
    JsonDocument &app = app_status();

    ptft->print("[a] ");
    Foreground_print_color(ptft, pu8f, TFT_BLACK);
    ptft->print("[b] ");
    Foreground_print_color(ptft, pu8f, TFT_WHITE);
    ptft->print("[c] ");
    Foreground_print_color(ptft, pu8f, TFT_NAVY);
    ptft->print("[d] ");
    Foreground_print_color(ptft, pu8f, TFT_DARKGREEN);
    ptft->print("[e] ");
    Foreground_print_color(ptft, pu8f, TFT_DARKCYAN);
    ptft->print("[f] ");
    Foreground_print_color(ptft, pu8f, TFT_MAROON);
    ptft->println();

    ptft->print("[g] ");
    Foreground_print_color(ptft, pu8f, TFT_PURPLE);
    ptft->print("[h] ");
    Foreground_print_color(ptft, pu8f, TFT_OLIVE);
    ptft->print("[i] ");
    Foreground_print_color(ptft, pu8f, TFT_LIGHTGREY);
    ptft->print("[j] ");
    Foreground_print_color(ptft, pu8f, TFT_DARKGREY);
    ptft->print("[k] ");
    Foreground_print_color(ptft, pu8f, TFT_BLUE);
    ptft->print("[l] ");
    Foreground_print_color(ptft, pu8f, TFT_GREEN);
    ptft->println();

    ptft->print("[m] ");
    Foreground_print_color(ptft, pu8f, TFT_CYAN);
    ptft->print("[n] ");
    Foreground_print_color(ptft, pu8f, TFT_RED);
    ptft->print("[o] ");
    Foreground_print_color(ptft, pu8f, TFT_MAGENTA);
    ptft->print("[p] ");
    Foreground_print_color(ptft, pu8f, TFT_YELLOW);
    ptft->print("[q] ");
    Foreground_print_color(ptft, pu8f, TFT_ORANGE);
    ptft->print("[r] ");
    Foreground_print_color(ptft, pu8f, TFT_GREENYELLOW);
    ptft->println();

    ptft->print("[s] ");
    Foreground_print_color(ptft, pu8f, TFT_PINK);
    ptft->print("[t] ");
    Foreground_print_color(ptft, pu8f, TFT_BROWN);
    ptft->print("[u] ");
    Foreground_print_color(ptft, pu8f, TFT_GOLD);
    ptft->print("[v] ");
    Foreground_print_color(ptft, pu8f, TFT_SILVER);
    ptft->print("[w] ");
    Foreground_print_color(ptft, pu8f, TFT_SKYBLUE);
    ptft->print("[x] ");
    Foreground_print_color(ptft, pu8f, TFT_VIOLET);
    ptft->println();

    //
    ptft->println();
}

//
void Foreground_keyboard(char key)
{
    JsonDocument &app = app_status();

    if (key == 'a')
    {
        app["config"]["foreground_color"] = TFT_BLACK;
        config_save();
    }
    else if (key == 'b')
    {
        app["config"]["foreground_color"] = TFT_WHITE;
        config_save();
    }
    else if (key == 'c')
    {
        app["config"]["foreground_color"] = TFT_NAVY;
        config_save();
    }
    else if (key == 'd')
    {
        app["config"]["foreground_color"] = TFT_DARKGREEN;
        config_save();
    }
    else if (key == 'e')
    {
        app["config"]["foreground_color"] = TFT_DARKCYAN;
        config_save();
    }
    else if (key == 'f')
    {
        app["config"]["foreground_color"] = TFT_MAROON;
        config_save();
    }
    else if (key == 'g')
    {
        app["config"]["foreground_color"] = TFT_PURPLE;
        config_save();
    }
    else if (key == 'h')
    {
        app["config"]["foreground_color"] = TFT_OLIVE;
        config_save();
    }
    else if (key == 'i')
    {
        app["config"]["foreground_color"] = TFT_LIGHTGREY;
        config_save();
    }
    else if (key == 'j')
    {
        app["config"]["foreground_color"] = TFT_DARKGREY;
        config_save();
    }
    else if (key == 'k')
    {
        app["config"]["foreground_color"] = TFT_BLUE;
        config_save();
    }
    else if (key == 'l')
    {
        app["config"]["foreground_color"] = TFT_GREEN;
        config_save();
    }
    else if (key == 'm')
    {
        app["config"]["foreground_color"] = TFT_CYAN;
        config_save();
    }
    else if (key == 'n')
    {
        app["config"]["foreground_color"] = TFT_RED;
        config_save();
    }
    else if (key == 'o')
    {
        app["config"]["foreground_color"] = TFT_MAGENTA;
        config_save();
    }
    else if (key == 'p')
    {
        app["config"]["foreground_color"] = TFT_YELLOW;
        config_save();
    }
    else if (key == 'q')
    {
        app["config"]["foreground_color"] = TFT_ORANGE;
        config_save();
    }
    else if (key == 'r')
    {
        app["config"]["foreground_color"] = TFT_GREENYELLOW;
        config_save();
    }
    else if (key == 's')
    {
        app["config"]["foreground_color"] = TFT_PINK;
        config_save();
    }
    else if (key == 't')
    {
        app["config"]["foreground_color"] = TFT_BROWN;
        config_save();
    }
    else if (key == 'u')
    {
        app["config"]["foreground_color"] = TFT_GOLD;
        config_save();
    }
    else if (key == 'v')
    {
        app["config"]["foreground_color"] = TFT_SILVER;
        config_save();
    }
    else if (key == 'w')
    {
        app["config"]["foreground_color"] = TFT_SKYBLUE;
        config_save();
    }
    else if (key == 'x')
    {
        app["config"]["foreground_color"] = TFT_VIOLET;
        config_save();
    }

    // go back to the word processor
    app["menu"]["state"] = MENU_HOME;
    app["screen"] = WORDPROCESSOR;
}
