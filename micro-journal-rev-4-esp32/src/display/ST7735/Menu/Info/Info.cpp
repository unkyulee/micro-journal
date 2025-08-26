#include "Info.h"
#include "../Menu.h"
#include "app/app.h"
#include "display/display.h"

#include "service/Editor/Editor.h"
#include "service/WordCounter/WordCounter.h"

//
void Info_setup(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    //
    JsonDocument &app = status();

    //
    Menu_clear();

    //
    int file_index = app["config"]["file_index"].as<int>();
    Editor::getInstance().loadFile(format("/%d.txt", file_index));

    //
    config_save();
}

//
void Info_render(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    //
    JsonDocument &app = status();
    int file_index = app["config"]["file_index"].as<int>();

    //
    int wordCountFile = app["config"][format("wordcount_file_%d", file_index)].as<int>();
    int wordCountBuffer = app["config"][format("wordcount_buffer_%d", file_index)].as<int>();

    //
    ptft->setCursor(0, 0);
    ptft->setTextColor(TFT_WHITE, TFT_BLACK);

    //
    ptft->setTextSize(1);
    ptft->println(format("Word Count %d.txt\n", file_index));

    ptft->setTextSize(3);
    ptft->println(format("%d", wordCountFile + wordCountBuffer));

    ptft->setTextSize(1);
    ptft->println(format("\n%d Bytes", Editor::getInstance().fileSize));
}

//
void Info_keyboard(char key, bool pressed)
{
    _debug("Info_Keyboard %d\n", key);

    // back to home menu
    JsonDocument &app = status();
    app["menu"]["state"] = MENU_HOME;
}