#include "Send.h"
#include "../Menu.h"
#include "app/app.h"
#include "display/display.h"
#include "display/ST7735/display_ST7735.h"

//
#include "service/Editor/Editor.h"
#include <Keyboard.h>

//
bool _sending = false;

//
void Send_setup(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    //
    Menu_clear();
}

//
void Send_render(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    //
    JsonDocument &app = status();

    //
    ptft->setTextSize(2);
    ptft->setTextColor(TFT_WHITE, TFT_BLACK);

    ptft->println(""); // Blank line if out of bounds
    ptft->println("Send Text");
    ptft->setTextSize(1);
    ptft->println("");
    ptft->println("Press any key to STOP");

    // this is incredible to be able tow write something that really tiny screen. eturn
    
    delay(1000);

    // send out text
    int file_index = app["config"]["file_index"].as<int>();
    Editor::getInstance().loadFile(format("/%d.txt", file_index));
    File file = gfs()->open(Editor::getInstance().fileName.c_str(), "r");
    if (!file || !file.available())
    {
        _log("Send_render: Failed to open file: %s\n", Editor::getInstance().fileName.c_str());
        return;
    }

    _log("Sending file content over keyboard...\n");

    // Read file content in small chunks
    _sending = true;
    while (file.available() && _sending)
    {
        char c = file.read();

        // If needed, convert line endings
        if (c == '\n')
        {
            Keyboard.write(KEY_RETURN);
        }
        else if (isPrintable(c) || c == '\t') // Avoid sending non-printable control characters
        {
            Keyboard.print(c);
        }

        // Small delay to prevent buffer overflow
        delay(5); // You can tweak this as needed
    }

    file.close();

    // end the sending session
    app["menu"]["state"] = MENU_HOME;

    _log("KeyboardScreen_copy: File sent.\n");
}

//
void Send_keyboard(char key, bool pressed)
{
    _debug("Stop sending text\n");

    // any key pressed is going to stop sending text
    _sending = false;

    JsonDocument &app = status();
    app["menu"]["state"] = MENU_HOME;
}
