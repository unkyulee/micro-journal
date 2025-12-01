#include "keypad_cardputer.h"
#include "keyboard/keyboard.h"
#include "keyboard/Locale/locale.h"

//
#include "app/app.h"

//
#include "display/display.h"

//
#include "M5Cardputer.h"

// initialize keymap
void keypad_cardputer_setup()
{
}

///
void keypad_cardputer_loop()
{
    M5Cardputer.update();
    if (M5Cardputer.Keyboard.isChange())
    {
        if (M5Cardputer.Keyboard.isPressed())
        {
            Keyboard_Class::KeysState status = M5Cardputer.Keyboard.keysState();

            // when FN key is pressed
            if (status.fn)
            { 
                // ESC
                if (M5Cardputer.Keyboard.isKeyPressed('`'))
                {
                    display_keyboard(27, true, 27);
                    display_keyboard(27, false, 27);
                }

                // DEL
                else if (status.del)
                {
                    display_keyboard(127, true, 127);
                    display_keyboard(127, false, 127);
                }

                // ARROW KEY - UP
                else if (M5Cardputer.Keyboard.isKeyPressed(';'))
                {
                    display_keyboard(20, true, 20);
                    display_keyboard(20, false, 20);
                }

                // ARROW KEY - DOWN
                else if (M5Cardputer.Keyboard.isKeyPressed('.'))
                {
                    display_keyboard(21, true, 21);
                    display_keyboard(21, false, 21);
                }

                // ARROW KEY - LEFT
                else if (M5Cardputer.Keyboard.isKeyPressed(','))
                {
                    display_keyboard(18, true, 18);
                    display_keyboard(18, false, 18);
                }

                // ARROW KEY - RIGHT
                else if (M5Cardputer.Keyboard.isKeyPressed('/'))
                {
                    display_keyboard(19, true, 19);
                    display_keyboard(19, false, 19);
                }
            }

            else
            {
                //
                for (auto i : status.word)
                {
                    // process caps lock
                    int ascii = keyboard_caplock_filter(i);
                    _log("[keypad_cardputer_loop] %d %d\n", i, ascii);

                    // i
                    display_keyboard(ascii, true, ascii);
                    display_keyboard(ascii, false, ascii);

                    // just take the first letter
                    break;
                }

                if(status.tab) {
                    // TAB -> CAPSLOCK
                    keyboard_capslock_toggle();
                }

                if (status.del)
                {
                    display_keyboard('\b', true, '\b');
                    display_keyboard('\b', false, '\b');
                }

                if (status.enter)
                {
                    display_keyboard('\n', true, '\n');
                    display_keyboard('\n', false, '\n');
                }
            }
        }
    }
}
