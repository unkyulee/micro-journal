# Quick Start Guide - Micro Journal Rev.4

Meet the Micro Journal Rev.4. Your keyboard's cooler, more focused sibling. At first glance, it's a sleek mechanical keyboard with hotswappable switches, ready to punch out emails or slay spreadsheets. But with just one click, it transforms into a distraction-free writerDeck. A minimalist writing sanctuary where notifications, tabs, and temptations vanish. Type your thoughts in peace, then send your masterpiece straight to your computer or phone when you're done. It's not just a keyboard. It's a portal to pure focus, cleverly disguised as one.


### Customizing Key Layout for writerDeck Mode

In writerDeck mode, the Micro Journal Rev.4 lets you take full control of your keyboard layout. Need special characters? Prefer a few accented Latin letters right at your fingertips? No problem. You can define your own custom layout by creating a simple JSON file. Just name it keyboard.json and save it to the device when it's in drive mode. Whether you're writing in multiple languages or just like things your way, your ideal layout is only a file away.

Here's how you can customize your keyboard layout:

1. **Create a "keyboard.json" file at drive mode.**
2. **Copy the following content into the "keyboard.json" file.**
3. **Turn off the Micro Journal and turn it on again.**
4. **Your customized keyboard layout will now be active.**


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
