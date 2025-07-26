#include "KeyboardScreen.h"
#include "app/app.h"
#include "display/display.h"
#include "service/GIF/GIF.h"
#include "keyboard/keyboard.h"

#include <Keyboard.h>

bool _keyboard_gif_loaded = false;
unsigned int _keyboard_sleep = 0;
bool _keyboard_is_sleep = false;
int _keyboard_awake_brightness = 0;
const int _keyboard_sleep_timer = 60 * 60 * 1000; // 1 hour


/*
Find the keycode from 
https://github.com/arduino-libraries/Keyboard/blob/master/src/Keyboard.h

LAYER KEY: #define KEY_F24           0xFB
*/
int _usb_keyboard_layers[2][72] = {

    {// normal layers
     KEY_ESC, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b', KEY_DELETE,
     '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', KEY_PAGE_UP,
     KEY_CAPS_LOCK, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '\\', KEY_PAGE_DOWN,
     KEY_LEFT_SHIFT, '`', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', KEY_LEFT_SHIFT, KEY_UP_ARROW, KEY_END,
     KEY_LEFT_CTRL, KEY_LEFT_GUI, KEY_LEFT_ALT, ' ', KEY_RIGHT_ALT, KEY_F24, KEY_HOME, KEY_LEFT_ARROW, KEY_DOWN_ARROW, KEY_RIGHT_ARROW,
     0},

    {// extra layer
     KEY_ESC, KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6, KEY_F7, KEY_F8, KEY_F9, KEY_F10, KEY_F11, KEY_F12, '\b', KEY_DELETE,
     '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', KEY_PRINT_SCREEN,
     KEY_CAPS_LOCK, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '\\', KEY_PAGE_DOWN,
     KEY_LEFT_SHIFT, '`', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', KEY_LEFT_SHIFT, KEY_UP_ARROW, KEY_END,
     KEY_LEFT_CTRL, KEY_LEFT_GUI, KEY_LEFT_ALT, ' ', KEY_RIGHT_ALT, KEY_F24, KEY_HOME, KEY_LEFT_ARROW, KEY_DOWN_ARROW, KEY_RIGHT_ARROW,
     0},

};

//
void KeyboardScreen_setup(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    JsonDocument &app = status();

    //
    ptft->fillScreen(TFT_BLACK);

    // load gif
    _keyboard_gif_loaded = gif_setup(ptft, pu8f, "/keyboard.gif", KEYBOARDSCREEN, false);

    // reset keyboard sleep timer
    _keyboard_sleep = millis();
    _keyboard_awake_brightness = app["config"]["brightness"].as<int>();

    // Load Custom Keybaord Layout
    keyboard_config_load("/keyboard_usb.json", (int *)_usb_keyboard_layers, 72);
}

//
void KeyboardScreen_render(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    // Play GIF
    if (_keyboard_gif_loaded)
        gif_render(ptft, pu8f);

    // Check Sleep Mode
    KeyboardScreen_sleep();
}

void KeyboardScreen_sleep()
{
    // check screen off mode: 1 hour inactive will turn off the screen
    if (millis() - _keyboard_sleep > _keyboard_sleep_timer && _keyboard_is_sleep == false)
    {
        _keyboard_sleep = millis();

        //
        JsonDocument &app = status();

        // go to sleep mode
        _keyboard_is_sleep = true;

        // turn off the screen
        app["config"]["brightness"] = 0;
        _log("Entering Sleep Mode\n");
    }
}


void KeyboardScreen_keyboard(int key, bool pressed, int index)
{
    JsonDocument &app = status();
    _debug("KeyboardScreen_keyboard - %d %d %d\n", key, pressed, index);

    // extend the sleep timer
    _keyboard_sleep = millis();

    // if keyboard is sleep then restore the brightness
    if (_keyboard_is_sleep)
    {
        // awake: restore the brightness
        app["config"]["brightness"] = _keyboard_awake_brightness;
    }

    // reset sleep flag
    _keyboard_is_sleep = false;

    //
    if (index == 69)
    {
        //
        if (!pressed)
        {
            // check if this is a long press
            // then toggle to writerDeck
            if (app["knobLongPressed"].as<bool>())
            {
                // open menu
                _debug("KeyboardScreen_keyboard - Received MENU Key\n");
                Keyboard.releaseAll();

                if (_keyboard_gif_loaded)
                    gif_stop(MENUSCREEN);
                else
                    app["screen"] = MENUSCREEN;
            }
            else
            {
                // move to writerDeck
                _debug("KeyboardScreen_keyboard - Received LONG PRESS MENU Key\n");
                Keyboard.releaseAll();

                // save the default screen to config
                app["config"]["UsbKeyboard"] = false;
                config_save();

                if (_keyboard_gif_loaded)
                    gif_stop(WAKEUPSCREEN);
                else
                    app["screen"] = WAKEUPSCREEN;
            }
        }

        return;
    }

    // Translate the key
    static int layer = 0;
    if (_usb_keyboard_layers[0][index] == KEY_F24)
    {
        if (pressed)
            layer = 1;
        else
            layer = 0;
        
        // layer key does not count a key press
        // ignore the layer key press
        return;
    }

    //
    int usbKey = _usb_keyboard_layers[layer][index];

    // Send out as a USB Keyboard Key Press
    if (pressed)
    {
        Keyboard.press(usbKey); // Press the key
    }
    else
    {
        Keyboard.release(usbKey); // Release the key
    }
}
