#include "FontColor.h"
#include "../Menu.h"
#include "app/app.h"
#include "display/display.h"
#include "display/ST7735/display_ST7735.h"

//
void FontColor_setup(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    //
    Menu_clear();
}

//
void FontColor_render(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    JsonDocument &app = status();
    int FontColor = app["config"]["foreground_color"].as<int>();

    ptft->setCursor(0, 0);
    ptft->setTextSize(2);
    ptft->setTextColor(TFT_WHITE, TFT_BLACK);

    ptft->println("Color Font");

    int length = sizeof(colors) / sizeof(colors[0]);
    int grid_cols = 6;
    int grid_rows = 4;
    int cell_w = 160 / grid_cols; // ≈26
    int cell_h = 60 / grid_rows;  // =15

    int start_x = 0;  // or center if you want: (ptft->width() - 160)/2
    int start_y = 20; // adjust as needed for your layout

    int selectedFontColorColorIndex = app["selectedFontColorColorIndex"].as<int>();

    for (int i = 0; i < length; i++)
    {
        int col = i % grid_cols;
        int row = i / grid_cols;
        int x = start_x + col * cell_w;
        int y = start_y + row * cell_h;
        ptft->fillRect(x, y, cell_w, cell_h, colors[i]);

        // if
        if (selectedFontColorColorIndex == i)
        {
            // draw a border
            ptft->drawRect(x, y, cell_w, cell_h, TFT_WHITE);
            ptft->drawRect(x + 1, y + 1, cell_w - 2, cell_h - 2, TFT_BLACK);
        }
    }
}

//
void FontColor_keyboard(char key, bool pressed)
{
    _debug("FontColor_Keyboard %d\n", key);
    JsonDocument &app = status();

    // ESC exit
    if (key == 27)
    {
        // Go back to Home
        _log("Exit FontColor without saving");

        //
        app["menu"]["state"] = MENU_HOME;
    }

    // MENU - SELECTED ACTION
    else if (key == 6 || key == '\n')
    {
        // Go back to Home
        _log("Exit FontColor");

        //
        app["menu"]["state"] = MENU_HOME;

        // Save Config
        app["config"]["foreground_color"] = colors[app["selectedFontColorColorIndex"].as<int>()];
        config_save();
    }

    // UP
    else if (key == 21 || key == 19)
    {
        //
        int selectedFontColorColorIndex = app["selectedFontColorColorIndex"].as<int>();
        selectedFontColorColorIndex++;
        if (selectedFontColorColorIndex > sizeof(colors) / sizeof(colors[0]))
            selectedFontColorColorIndex = 0;

        app["selectedFontColorColorIndex"] = selectedFontColorColorIndex;
    }

    // DOWN
    else if (key == 20 || key == 18)
    {
        //
        int selectedFontColorColorIndex = app["selectedFontColorColorIndex"].as<int>();
        selectedFontColorColorIndex--;
        if (selectedFontColorColorIndex < 0)
            selectedFontColorColorIndex = sizeof(colors) / sizeof(colors[0]) - 1;

        app["selectedFontColorColorIndex"] = selectedFontColorColorIndex;
    }
}
