#include "Background.h"
#include "../Menu.h"
#include "app/app.h"
#include "display/display.h"
#include "display/ST7735/display_ST7735.h"

//
void Background_setup(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    //
    Menu_clear();
}

//
void Background_render(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    JsonDocument &app = status();
    int Background = app["config"]["Background"].as<int>();

    ptft->setCursor(0, 0);
    ptft->setTextSize(2);
    ptft->setTextColor(TFT_WHITE, TFT_BLACK);

    ptft->println("Color BG");

    int length = sizeof(colors) / sizeof(colors[0]);
    int grid_cols = 6;
    int grid_rows = 4;
    int cell_w = 160 / grid_cols; // ≈26
    int cell_h = 60 / grid_rows;  // =15

    int start_x = 0;  // or center if you want: (ptft->width() - 160)/2
    int start_y = 20; // adjust as needed for your layout

    int selectedBackgroundColorIndex = app["selectedBackgroundColorIndex"].as<int>();

    for (int i = 0; i < length; i++)
    {
        int col = i % grid_cols;
        int row = i / grid_cols;
        int x = start_x + col * cell_w;
        int y = start_y + row * cell_h;
        ptft->fillRect(x, y, cell_w, cell_h, colors[i]);

        // if
        if (selectedBackgroundColorIndex == i)
        {
            // draw a border
            ptft->drawRect(x, y, cell_w, cell_h, TFT_WHITE);
            ptft->drawRect(x + 1, y + 1, cell_w - 2, cell_h - 2, TFT_BLACK);
        }
    }
}

//
void Background_keyboard(char key, bool pressed)
{
    _debug("Background_Keyboard %d\n", key);
    JsonDocument &app = status();

    // MENU - SELECTED ACTION
    if (key == 6 || key == '\n')
    {
        // Go back to Home
        _log("Exit Background");

        // Save Config
        config_save();

        //
        app["menu"]["state"] = MENU_HOME;
    }

    // UP
    else if (key == 21|| key == 19)
    {
        //
        int selectedBackgroundColorIndex = app["selectedBackgroundColorIndex"].as<int>();
        selectedBackgroundColorIndex++;
        if (selectedBackgroundColorIndex > sizeof(colors) / sizeof(colors[0]))
            selectedBackgroundColorIndex = 0;

        app["selectedBackgroundColorIndex"] = selectedBackgroundColorIndex;
        app["config"]["background_color"] = colors[selectedBackgroundColorIndex];
    }

    // DOWN
    else if (key == 20 || key == 18)
    {
        //
        int selectedBackgroundColorIndex = app["selectedBackgroundColorIndex"].as<int>();
        selectedBackgroundColorIndex--;
        if (selectedBackgroundColorIndex < 0)
            selectedBackgroundColorIndex = sizeof(colors) / sizeof(colors[0]) - 1;

        app["selectedBackgroundColorIndex"] = selectedBackgroundColorIndex;
        app["config"]["background_color"] = colors[selectedBackgroundColorIndex];
    }
}
