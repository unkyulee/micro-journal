# Quick Start Guide - Micro Journal Rev.4

Meet the Micro Journal Rev.4. Your keyboard's cooler, more focused sibling. At first glance, it's a sleek mechanical keyboard with hotswappable switches, ready to punch out emails or slay spreadsheets. But with just one click, it transforms into a distraction-free writerDeck. A minimalist writing sanctuary where notifications, tabs, and temptations vanish. Type your thoughts in peace, then send your masterpiece straight to your computer or phone when you're done. It's not just a keyboard. It's a portal to pure focus, cleverly disguised as one.


### Customizing Key Layout for writerDeck Mode

In writerDeck mode, the Micro Journal Rev.4 lets you take full control of your keyboard layout. Need special characters? Prefer a few accented Latin letters right at your fingertips? No problem. You can define your own custom layout by creating a simple JSON file. Just name it keyboard.json and save it to the device when it's in drive mode. Whether you're writing in multiple languages or just like things your way, your ideal layout is only a file away.


```json
{
    "main": 
    [
     "ESC", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "-", "=", "BACKSPACE", "DEL",
     " ", "q", "w", "e", "r", "t", "y", "u", "i", "o", "p", "[", "]", "\n", "PGUP",
     0, "a", "s", "d", "f", "g", "h", "j", "k", "l", ";", "\"", "\\", "PGDN",
     "SHIFT", "`", "z", "x", "c", "v", "b", "n", "m", ",", ".", "/", "SHIFT", "UP", "END",
     "ALT", "ALT", "ALT", " ", "ALT", "ALT", "HOME", "LEFT", "DOWN", "RIGHT",
     "MENU"
    ],

    "main-shift": 
    [
     "ESC", "!", "@", "#", "$", "%", "^", "&", "*", "(", ")", "_", "+", "BACKSPACE", "DEL",
     " ", "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P", "{", "}", "\n", "PGUP",
     0, "A", "S", "D", "F", "G", "hH", "J", "K", "L", ":", "'", "|", "PGDN",
     "SHIFT", "~", "Z", "X", "C", "V", "B", "N", "M", "<", ">", "?", "SHIFT", "UP", "END",
     "ALT", "ALT", "ALT", " ", "ALT", "ALT", "HOME", "LEFT", "DOWN", "RIGHT",
     "MENU"
    ],

    "alt": 
    [
     "ESC", "FILE1", "FILE2", "FILE3", "FILE4", "FILE5", "FILE6", "FILE7", "FILE8", "FILE9", "FILE0", "-", "=", "BACKSPACE", "DEL",
     " ", "q", "w", "e", "r", "t", "y", "u", "i", "o", "p", "[", "]", "\n", "PGUP",
     0, "a", "s", "d", "f", "g", "h", "j", "k", "l", ";", "\"", "\\", "PGDN",
     "SHIFT", "`", "z", "x", "c", "v", "b", "n", "m", ",", ".", "/", "SHIFT", "UP", "END",
     "ALT", "ALT", "ALT", " ", "ALT", "ALT", "HOME", "LEFT", "DOWN", "RIGHT",
     "MENU"
    ],

    "alt-shift": 
    [
     "ESC", "FILE1", "FILE2", "FILE3", "FILE4", "FILE5", "FILE6", "FILE7", "FILE8", "FILE9", "FILE0", "-", "=", "BACKSPACE", "DEL",
     " ", "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P", "{", "}", "\n", "PGUP",
     0, "A", "S", "D", "F", "G", "hH", "J", "K", "L", ":", "'", "|", "PGDN",
     "SHIFT", "~", "Z", "X", "C", "V", "B", "N", "M", "<", ">", "?", "SHIFT", "UP", "END",
     "ALT", "ALT", "ALT", " ", "ALT", "ALT", "HOME", "LEFT", "DOWN", "RIGHT",
     "MENU"
    ]
}
```

Here's how you can customize your keyboard layout:

1. **Create a "keyboard.json" file at drive mode.**
2. **Copy the following content into the "keyboard.json" file.**
3. **Turn off the Micro Journal and turn it on again.**
4. **Your customized keyboard layout will now be active.**


[Download other writerDeck keyboard layout](https://github.com/unkyulee/micro-journal/tree/main/micro-journal-rev-4-revamp/keyboard_template)

- **"main"**: Key pressed without any modifiers (no Shift, no Alt).
- **"main-shift"**: Key pressed while holding the Shift key.
- **"alt"**: Key pressed while holding the Alt key.
- **"alt-shift"**: Key pressed while holding both the Alt and Shift keys.

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
| FILE0 ~ 9              | Transition to the target file             |
| MENU                   | Enter menu (assigned to knob click)       |

For accented keys, you can't directly write characters like è or ù in the JSON file as they won’t be recognized. Instead, you need to use their DEC values from the ASCII Table, which can be found at [ASCII Code](https://www.ascii-code.com/).

For example:
- È - 200
- è - 232



### Customizing Key Layout for Keyboard Mode

When using the Micro Journal Rev.4 in USB keyboard mode, what you type will appear on your computer screen. Exactly what appears depends on your operating system's language and keyboard settings. For example, if your system is set to a US layout, the keys you press will match what you see in the layout configuration. However, switch your system to Italian, and pressing the key where ";" usually lives will type "ò" instead. There's no need to reconfigure the keyboard itself in most of the cases. Just set your OS language, and you're good to go.

The keyboard also features two configurable layers. By pressing the LAYER key, you can access a hidden layer of functions. In the default setup, this allows the number row to double as F1 - F12 keys, filling in for a standard keyboard's missing function keys. Other useful remaps include turning PAGE UP into Print Screen, and you can easily assign more hidden functions as needed.

Thinking of using a layout like DVORAK? That's possible too. Simply remap the letters to their desired positions, and your custom configuration is ready to type.


```json
{
    "main": 
    [
     "\t", "q", "w", "e", "r", "t", "y", "u", "i", "o", "p", "\b",
     "ESC", "a", "s", "d", "f", "g", "h", "j", "k", "l", ";", "\"",
     "LEFT_SHIFT", "z", "x", "c", "v", "b", "n", "m", ",", ".", "/", "\n",
     0, "LEFT_CTRL", "LEFT_ALT", "LEFT_GUI", "LOWER", " ", " ", "RAISE", "LEFT_ARROW", "DOWN_ARROW", "UP_ARROW", "RIGHT_ARROW"
    ],

    "lower": 
    [
     "~", "!", "@", "#", "$", "%", "^", "&", "*", "(", ")", "DELETE",
     "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10", "F11", "F12",
     "LEFT_SHIFT", "_", "+", "{", "}", "|", "n", "m", ",", ".", "/", "\n",
     "MENU", "LEFT_CTRL", "LEFT_ALT", "LEFT_GUI", "LOWER", " ", " ", "RAISE", "HOME", "PAGE_DOWN", "PAGE_UP", "END"
    ],

    "raise": 
    [
     "`", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "\b",
     "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10", "F11", "F12",
     "LEFT_SHIFT", "-", "=", "[", "]", "\\", "n", "m", ",", ".", "/", "\n",
     "MENU", "LEFT_CTRL", "LEFT_ALT", "LEFT_GUI", "LOWER", " ", " ", "RAISE", "PRINT_SCREEN", "PAGE_DOWN", "PAGE_UP", "END"
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

 