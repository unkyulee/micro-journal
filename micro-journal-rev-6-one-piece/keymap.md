# Keyboard Layout Customization Guide

Customizing your keyboard layout is straightforward and involves placing a "keyboard.json" file in the SD card. 

Here's how you can customize your keyboard layout:

1. **Create a "keyboard.json" file inside the SD card.**
2. **Copy the following content into the "keyboard.json" file.**
3. **Turn off the Micro Journal and turn it on again.**
4. **Your customized keyboard layout will now be active.**


The following json is the default configuration. 

```json
{
    "main": 
    [
     "MENU", "q", "w", "e", "r", "t", "y", "u", "i", "o", "p", "BACKSPACE",
     "\\", "a", "s", "d", "f", "g", "h", "j", "k", "l", ";", "'",
     "SHIFT", "z", "x", "c", "v", "b", "n", "m", ",", ".", "/", "SHIFT",
     "-", "=", "[", "]", "LOWER", " ", " ", "LEFT", "DOWN", "UP", "RIGHT", "\n"
    ],

    "main-shift": 
    [
     "MENU", "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P", "DEL",
     "|", "A", "S", "D", "F", "G", "H", "J", "K", "L", ":", "\"",
     "SHIFT", "Z", "X", "C", "V", "B", "N", "M", "<", ">", "?", "SHIFT",
     "_", "+", "{", "}", "LOWER", " ", " ", "HOME", "PGDN", "PGUP", "END", "\n"
    ],

    "lower": 
    [
     "MENU", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "DEL",
     "`", "a", "s", "d", "f", "g", "h", "j", "k", "l", ";", "'",
     "SHIFT", "z", "x", "c", "v", "b", "n", "m", ",", ".", "/", "SHIFT",
     "-", "=", "[", "]", "LOWER", " ", " ", "HOME", "PGDN", "PGUP", "END", "\n"
    ],

    "lower-shift": 
    [
     "MENU", "!", "@", "#", "$", "%", "^", "&", "*", "(", ")", "DEL",
     "~", "A", "S", "D", "F", "G", "H", "J", "K", "L", ":", "\"",
     "SHIFT", "Z", "X", "C", "V", "B", "N", "M", "<", ">", "?", "SHIFT",
     "_", "+", "{", "}", "LOWER", " ", " ", "HOME", "PGDN", "PGUP", "END", "\n"
    ],

    "raise": 
    [
     "MENU", 1001, 1002, 1003, 1004, 1005, 1006, 1007, 1008, 1009, 1000, "DEL",
     "`", "a", "s", "d", "f", "g", "h", "j", "k", "l", ";", "'",
     "SHIFT", "z", "x", "c", "v", "b", "n", "m", ",", ".", "/", "SHIFT",
     "-", "=", "[", "]", "LOWER", " ", " ", "HOME", "PGDN", "PGUP", "END", "\n"
    ],

    "raise-shift": 
    [
     "MENU", 1001, 1002, 1003, 1004, 1005, 1006, 1007, 1008, 1009, 1000, "DEL",
     "~", "A", "S", "D", "F", "G", "H", "J", "K", "L", ":", "\"",
     "SHIFT", "Z", "X", "C", "V", "B", "N", "M", "<", ">", "?", "SHIFT",
     "_", "+", "{", "}", "LOWER", " ", " ", "HOME", "PGDN", "PGUP", "END", "\n"
    ]
}
```

- **"main"**: Key pressed without any modifiers (no Shift, no Alt).
- **"main-shift"**: Key pressed while holding the Shift key.
- **"alt"**: Key pressed while holding the Alt key.
- **"alt-shift"**: Key pressed while holding both the Alt and Shift keys.

This configuration allows for a total of 96 keys on the keyboard, and with Shift key combinations, it can extend to around 192 key layouts.

Most keys are typed directly into the JSON file between quotes. However, some keys are special and need specific representations. Here is a list of special keys and their corresponding representations:

| Special Key Written AS | Description                               |
| ---------------------- | ----------------------------------------- |
| ESC                    | Functions as entering the MENU screen     |
| SHIFT                  | This is a SHIFT key                       |
| ALT                    | Hidden layer activation key               |
| BACKSPACE              | Works as a backspace                      |
| 127                    | Deletes a word (DEL)                      |
| \n                     | Enter Key                                 |
| \\\\                   | Backslash                                 |
| \\"                    | Double Quote                              |
| LEFT                   | Navigate LEFT                             |
| RIGHT                  | Navigate RIGHT                            |
| UP                     | Navigate UP                               |
| DOWN                   | Navigate DOWN                             |
| PGUP                   | Go to Previous Page                       |
| PGDN                   | Go to Next Page                           |
| HOME                   | Move to the beginning of the current line |
| END                    | Move to the end the current line          |
| 1000 ~ 1009            | Change File                               |

For accented keys, you can't directly write characters like è or ù in the JSON file as they won’t be recognized. Instead, you need to use their DEC values from the ASCII Table, which can be found at [ASCII Code](https://www.ascii-code.com/).

For example:
- È - 200
- è - 232


In the above example. The key next to the backspace will type è and È respectively. 
Note that it is written in a number and not in the actual character. 

Remember to replace the placeholder values with your desired key configurations. This process enables you to tailor the keyboard layout to your specific needs.



### Customizing Key Layout for Keyboard Mode

When using the Micro Journal Rev.4 in USB keyboard mode, what you type will appear on your computer screen. Exactly what appears depends on your operating system's language and keyboard settings. For example, if your system is set to a US layout, the keys you press will match what you see in the layout configuration. However, switch your system to Italian, and pressing the key where ";" usually lives will type "ò" instead. There's no need to reconfigure the keyboard itself in most of the cases. Just set your OS language, and you're good to go.

The keyboard also features two configurable layers. By pressing the LAYER key, you can access a hidden layer of functions. In the default setup, this allows the number row to double as F1 - F12 keys, filling in for a standard keyboard's missing function keys. Other useful remaps include turning PAGE UP into Print Screen, and you can easily assign more hidden functions as needed.

Thinking of using a layout like DVORAK? That's possible too. Simply remap the letters to their desired positions, and your custom configuration is ready to type.


```json
{
    "main": 
    [
     "ESC", "q", "w", "e", "r", "t", "y", "u", "i", "o", "p", "\b",
     "\t", "a", "s", "d", "f", "g", "h", "j", "k", "l", ";", "\"",
     "LEFT_SHIFT", "z", "x", "c", "v", "b", "n", "m", ",", ".", "/", "RIGHT_SHIFT",
     "LEFT_CTRL", "LEFT_GUI", "LEFT_ALT", "RAISE", "LOWER", " ", " ", "LEFT_ARROW", "DOWN_ARROW", "UP_ARROW", "RIGHT_ARROW", "\n"
    ],

    "lower": 
    [
     "ESC", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "DELETE",
     "`", "a", "s", "d", "f", "g", "h", "j", "-", "=", "[", "]",
     "LEFT_SHIFT", "z", "x", "c", "v", "b", "n", "m", ",", ".", "/", "RIGHT_SHIFT",
     "LEFT_CTRL", "LEFT_GUI", "LEFT_ALT", "RAISE", "LOWER", " ", " ", "HOME", "PAGE_DOWN", "PAGE_UP", "END", "\n"
    ],

    "raise": 
    [
     "ESC", "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10", "DELETE",
     "\\", "a", "s", "d", "f", "g", "h", "j", "-", "=", "[", "]",
     "LEFT_SHIFT", "z", "x", "c", "v", "b", "n", "m", ",", ".", "/", "PRINT_SCREEN",
     "LEFT_CTRL", "LEFT_GUI", "LEFT_ALT", "RAISE", "LOWER", " ", " ", "HOME", "PAGE_DOWN", "PAGE_UP", "END", "\n"
    ]
}
```

Here's how you can customize your keyboard layout:

1. **Create a "keyboard_usb.json" file at sd card.**
2. **Copy the following content into the "keyboard_usb.json" file.**
3. **Turn off the Micro Journal and turn it on again.**
4. **Your customized keyboard layout will now be active.**


There are 3 layers in total.
- **"main"**: Key pressed without any modifiers (no Shift, no Alt).
- **"lower"**: Key pressed while holding the LOWER key.
- **"raise"**: Key pressed while holding the RAISE key.


[Download other USB keyboard layout](https://github.com/unkyulee/micro-journal/tree/main/micro-journal-rev-4-revamp/keyboard_usb_template)

Keywords used in the template follows the names from the following link

https://github.com/arduino-libraries/Keyboard/blob/master/src/Keyboard.h

 