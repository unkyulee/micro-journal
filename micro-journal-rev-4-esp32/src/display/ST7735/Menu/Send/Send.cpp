#include "Send.h"
#include "../Menu.h"
#include "app/app.h"
#include "display/display.h"
#include "display/ST7735/display_ST7735.h"

//
#include "service/Editor/Editor.h"
#include "service/Send/Send.h"

//
#include <Keyboard.h>

// function reference used for SEND
void _send_key(int key)
{
    // If needed, convert line endings
    char c = (char)key;
    if (c == '\n')
    {
        Keyboard.write(KEY_RETURN);
    }
    else if (isPrintable(c) || c == '\t') // Avoid sending non-printable control characters
    {
        Keyboard.print(c);
    }
}

//
void Send_setup(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    //
    Menu_clear();

    // register send function
    send_register(_send_key);

    // request SEND to background task
    JsonDocument &app = status();
    app["task"] = "send_start";
    app["send_stop"] = false;
    app["send_finished"] = false;
}

//
void Send_render(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    //
    JsonDocument &app = status();

    //
    ptft->setCursor(0, 0);
    ptft->setTextSize(2);
    ptft->setTextColor(TFT_WHITE, TFT_BLACK);

    ptft->println(""); // Blank line if out of bounds
    ptft->println("Send Text");
    ptft->setTextSize(1);
    ptft->println("");
    ptft->println("Press any key to STOP");

    // monitor if the send has finished
    if (app["send_finished"].as<bool>())
    {
        _log("Send is completed. Moving back to the previous screen.\n");
        
        // end the sending session
        bool UsbKeyboard = app["config"]["UsbKeyboard"].as<bool>();
        if (UsbKeyboard)
        {
            app["screen"] = KEYBOARDSCREEN;
        }
        else
        {
            app["screen"] = WORDPROCESSOR;
        }
    }
}

//
void Send_keyboard(char key, bool pressed)
{
    _debug("Stop sending text\n");

    // any key pressed is going to stop sending text
    JsonDocument &app = status();
    app["send_stop"] = true;
}
