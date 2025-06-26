#include "Background.h"
#include "../Menu.h"
#include "app/app.h"
#include "config/config.h"
#include "display/display.h"

//
#include "../../WordProcessor/WordProcessor.h"

//
void Background_setup(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    // when entering the screen
    // clear the screen
    Menu_clear();
}

void Background_print_color(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f, uint16_t color)
{
    int cursorX = ptft->getCursorX();
    int cursorY = ptft->getCursorY();
    ptft->fillCircle(cursorX + 8, cursorY + 8, 6, TFT_WHITE);
    ptft->fillCircle(cursorX + 8, cursorY + 8, 5, color);
    ptft->setCursor(cursorX + 25, cursorY);
}

//
void Background_render(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    ptft->setCursor(0, 30, 2);
    ptft->setTextSize(1);
    //
    ptft->setTextColor(TFT_WHITE, TFT_BLACK);
    ptft->println(" BACKGROUND COLOR");
    ptft->println("");

    //
    JsonDocument &app = app_status();

    ptft->print("[a] ");
    Background_print_color(ptft, pu8f, TFT_BLACK);
    ptft->print("[b] ");
    Background_print_color(ptft, pu8f, TFT_WHITE);
    ptft->print("[c] ");
    Background_print_color(ptft, pu8f, TFT_NAVY);
    ptft->print("[d] ");
    Background_print_color(ptft, pu8f, TFT_DARKGREEN);
    ptft->print("[e] ");
    Background_print_color(ptft, pu8f, TFT_DARKCYAN);
    ptft->print("[f] ");
    Background_print_color(ptft, pu8f, TFT_MAROON);
    ptft->println();

    ptft->print("[g] ");
    Background_print_color(ptft, pu8f, TFT_PURPLE);
    ptft->print("[h] ");
    Background_print_color(ptft, pu8f, TFT_OLIVE);
    ptft->print("[i] ");
    Background_print_color(ptft, pu8f, TFT_LIGHTGREY);
    ptft->print("[j] ");
    Background_print_color(ptft, pu8f, TFT_DARKGREY);
    ptft->print("[k] ");
    Background_print_color(ptft, pu8f, TFT_BLUE);
    ptft->print("[l] ");
    Background_print_color(ptft, pu8f, TFT_GREEN);
    ptft->println();

    ptft->print("[m] ");
    Background_print_color(ptft, pu8f, TFT_CYAN);
    ptft->print("[n] ");
    Background_print_color(ptft, pu8f, TFT_RED);
    ptft->print("[o] ");
    Background_print_color(ptft, pu8f, TFT_MAGENTA);
    ptft->print("[p] ");
    Background_print_color(ptft, pu8f, TFT_YELLOW);
    ptft->print("[q] ");
    Background_print_color(ptft, pu8f, TFT_ORANGE);
    ptft->print("[r] ");
    Background_print_color(ptft, pu8f, TFT_GREENYELLOW);
    ptft->println();

    ptft->print("[s] ");
    Background_print_color(ptft, pu8f, TFT_PINK);
    ptft->print("[t] ");
    Background_print_color(ptft, pu8f, TFT_BROWN);
    ptft->print("[u] ");
    Background_print_color(ptft, pu8f, TFT_GOLD);
    ptft->print("[v] ");
    Background_print_color(ptft, pu8f, TFT_SILVER);
    ptft->print("[w] ");
    Background_print_color(ptft, pu8f, TFT_SKYBLUE);
    ptft->print("[x] ");
    Background_print_color(ptft, pu8f, TFT_VIOLET);
    ptft->println();

    //
    ptft->println();
}

//
void Background_keyboard(char key)
{
    JsonDocument &app = app_status();

    if (key == 'a')
    {
        app["config"]["background_color"] = TFT_BLACK;
        config_save();
    }
    else if (key == 'b')
    {
        app["config"]["background_color"] = TFT_WHITE;
        config_save();
    }
    else if (key == 'c')
    {
        app["config"]["background_color"] = TFT_NAVY;
        config_save();
    }
    else if (key == 'd')
    {
        app["config"]["background_color"] = TFT_DARKGREEN;
        config_save();
    }
    else if (key == 'e')
    {
        app["config"]["background_color"] = TFT_DARKCYAN;
        config_save();
    }
    else if (key == 'f')
    {
        app["config"]["background_color"] = TFT_MAROON;
        config_save();
    }
    else if (key == 'g')
    {
        app["config"]["background_color"] = TFT_PURPLE;
        config_save();
    }
    else if (key == 'h')
    {
        app["config"]["background_color"] = TFT_OLIVE;
        config_save();
    }
    else if (key == 'i')
    {
        app["config"]["background_color"] = TFT_LIGHTGREY;
        config_save();
    }
    else if (key == 'j')
    {
        app["config"]["background_color"] = TFT_DARKGREY;
        config_save();
    }
    else if (key == 'k')
    {
        app["config"]["background_color"] = TFT_BLUE;
        config_save();
    }
    else if (key == 'l')
    {
        app["config"]["background_color"] = TFT_GREEN;
        config_save();
    }
    else if (key == 'm')
    {
        app["config"]["background_color"] = TFT_CYAN;
        config_save();
    }
    else if (key == 'n')
    {
        app["config"]["background_color"] = TFT_RED;
        config_save();
    }
    else if (key == 'o')
    {
        app["config"]["background_color"] = TFT_MAGENTA;
        config_save();
    }
    else if (key == 'p')
    {
        app["config"]["background_color"] = TFT_YELLOW;
        config_save();
    }
    else if (key == 'q')
    {
        app["config"]["background_color"] = TFT_ORANGE;
        config_save();
    }
    else if (key == 'r')
    {
        app["config"]["background_color"] = TFT_GREENYELLOW;
        config_save();
    }
    else if (key == 's')
    {
        app["config"]["background_color"] = TFT_PINK;
        config_save();
    }
    else if (key == 't')
    {
        app["config"]["background_color"] = TFT_BROWN;
        config_save();
    }
    else if (key == 'u')
    {
        app["config"]["background_color"] = TFT_GOLD;
        config_save();
    }
    else if (key == 'v')
    {
        app["config"]["background_color"] = TFT_SILVER;
        config_save();
    }
    else if (key == 'w')
    {
        app["config"]["background_color"] = TFT_SKYBLUE;
        config_save();
    }
    else if (key == 'x')
    {
        app["config"]["background_color"] = TFT_VIOLET;
        config_save();
    }

    // go back to the word processor
    app["menu"]["state"] = MENU_HOME;
    app["screen"] = WORDPROCESSOR;
}
