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
    int wordCountFile = wordcounter_file(Editor::getInstance().fileName.c_str());
    int wordCountBuffer = wordcounter_buffer(Editor::getInstance().buffer);

    // svae the word count
    app["config"][format("wordcount_file_%d", file_index)] = wordCountFile;
    app["config"][format("wordcount_buffer_%d", file_index)] = wordCountBuffer;

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
    ptft->setCursor(0, 0);
    ptft->setTextColor(TFT_WHITE, TFT_BLACK);

    //
    ptft->setTextSize(1);
    ptft->println(format("Word Count %d.txt\n", file_index));

    ptft->setTextSize(3);
    String key = format("wordcount_%d", file_index);
    ptft->println(format("%d", app["config"][key].as<int>()));

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