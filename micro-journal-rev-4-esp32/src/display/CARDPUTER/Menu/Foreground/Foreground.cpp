#include "Foreground.h"
#include "../Menu.h"
#include "app/app.h"
#include "display/display.h"
#include "../../WordProcessor/WordProcessor.h"

//
#include "display/CARDPUTER/display_CARDPUTER.h"


//
void Foreground_setup()
{
    // when entering the screen
    // clear the screen
    Menu_clear();
}

//
void Foreground_render()
{
    JsonDocument &app = status();

    //
    M5Cardputer.Display.setTextSize(0.5);
    M5Cardputer.Display.setFont(&fonts::Orbitron_Light_24);

    M5Cardputer.Display.drawString("FONT COLOR ", 0, 0);

    int length = sizeof(colors) / sizeof(colors[0]);
    int grid_cols = 6;
    int grid_rows = 4;

    int start_x = 0;  // or center if you want: (ptft->width() - 160)/2
    int start_y = 14; // adjust as needed for your layout

    int cell_w = M5Cardputer.Display.width() / grid_cols;              // ≈26
    int cell_h = (M5Cardputer.Display.height() - start_y) / grid_rows; // =15

    int selectedFontColorColorIndex = app["selectedFontColorColorIndex"].as<int>();

    for (int i = 0; i < length; i++)
    {
        int col = i % grid_cols;
        int row = i / grid_cols;
        int x = start_x + col * cell_w;
        int y = start_y + row * cell_h;
        M5Cardputer.Display.fillRect(x, y, cell_w, cell_h, colors[i]);

        // if
        if (selectedFontColorColorIndex == i)
        {
            // draw a border
            M5Cardputer.Display.drawRect(x, y, cell_w, cell_h, TFT_WHITE);
            M5Cardputer.Display.drawRect(x + 1, y + 1, cell_w - 2, cell_h - 2, TFT_BLACK);
        }
    }
}

//
void Foreground_keyboard(char key)
{
     JsonDocument &app = status();

    // ESC exit
    if (key == 27)
    {
        // Go back to Home
        _log("Exit FontColor without saving");

        //
        app["menu"]["state"] = MENU_HOME;
    }

    // Enter - SELECTED ACTION
    else if (key == '\n')
    {
        // Go back to Home
        _log("Exit Background");

        //
        app["menu"]["state"] = MENU_HOME;

        // Save Config
        app["config"]["foreground_color"] = colors[app["selectedFontColorColorIndex"].as<int>()];
        config_save();
    }

    // UP
    else if (key == 21 || key == 19 || key == '.' || key == '/')
    {
        //
        int selectedFontColorColorIndex = app["selectedFontColorColorIndex"].as<int>();
        selectedFontColorColorIndex++;
        if (selectedFontColorColorIndex > sizeof(colors) / sizeof(colors[0]))
            selectedFontColorColorIndex = 0;

        app["selectedFontColorColorIndex"] = selectedFontColorColorIndex;
    }

    // DOWN
    else if (key == 20 || key == 18 || key == ';' || key == ',')
    {
        //
        int selectedFontColorColorIndex = app["selectedFontColorColorIndex"].as<int>();
        selectedFontColorColorIndex--;
        if (selectedFontColorColorIndex < 0)
            selectedFontColorColorIndex = sizeof(colors) / sizeof(colors[0]) - 1;

        app["selectedFontColorColorIndex"] = selectedFontColorColorIndex;
    }
}
