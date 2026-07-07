# Micro Journal Rev.8 Quick Start Guide 

At the moment, the documentation is still in preparation.



## First Steps & Setup

### Battery Installation
- (not included) Requires **one 18650 Li-ion 3.7V battery**.  
- Capacity should not exceed **3300 mAh**.  
- ⚠️ Insert with correct polarity (+ and –).  
- Charge fully before first use (red LED = charging, blue LED = charged).  
- Use USB-A to USB-C chargers (PD chargers may not work).  
- [Battery Buying Guide](https://www.reddit.com/r/18650masterrace/comments/qp21o8/buying_18650_batteries_start_here/?share_id=RTqe02Hqv2l_VwJTwoIi1&utm_content=2&utm_medium=android_app&utm_name=androidcss&utm_source=share&utm_term=1)
- [This Battery Works, Flat-top (US)](https://www.18650batterystore.com/products/samsung-30q)

⚠️ Wrong polarity can cause **irreparable damage or fire**.

⚠️ You will also need TORX T10H hex screw driver in order to open up the display lid in order to place the battery

⚠️ When the 'P' and 'Backspace' Keys Aren't Working. This issue usually occurs if the battery hasn't been installed or if the USB cable is plugged into the wrong port. Make sure not to use the USB-C port on the lower left side, which is closer to the keyboard. There are two USB-C ports on the left side of the device; always use the upper USB-C port to power the device properly.



### Firmware Update

Back up all the files inside the Drive. 
Use "Drive Mode" and copy all the files before this process. 
All files will be deleted during the firmware update.

1) Download the firmware to your PC
https://github.com/unkyulee/micro-journal/releases

2) Go to the web flash tool:
https://www.espboards.dev/tools/program/

3) Connect the ESP32 port to your computer.
When the screen is open, use the upper USB port.

4) Click Connect in the web flash tool.

5) Upload the file named firmware_rev_8.bin.

6) Change the Flash Address to "0x0"

7) Click Program to flash the firmware.



## Moving the Text

- Connect USB cable to the ESP32 (side of the display) bottom port to a PC. 
- In Micro Journal Rev.8 menu, select "Drive Mode"
- In PC, drive will show up and you can navigate text files inside the drive
- Click on "Eject" from the PC or Press "ESC" from Micro Journal Rev.8
- USB cable can be disconnected


You can also use "Send" feature after pairing BLE keyboard to a PC or a phone.
You can also setup a Google Drive Sync to back up your files to Google Drive

- [How to Setup Google Drive Sync](../micro-journal-rev-6-one-piece/googledrive.md)



## Modifying the Rev.8's Key Maps

⚠️ Currently under revision. Custom keymaps may not work exactly as mentioned below. Should be updated soon.


The Key Maps, or keyboard layout, determines what output is obtained when a key or combination of keys are pressed or held. The most obvious key map is what is printed on your keyboard key caps. But you can change some of those assignments and The Rev 8 provides additional layers so you can have access to even more kinds of keys. In fact, there are two different keyboard maps for the Rev 4: the Key Map for Computer Keyboard Mode and the writerDeck Mode.

The key maps are determined by special text files with the extension of .json (JavaScript Object Notation). It is a way of representing structured data to the computer, in this case Key Maps. This walkthrough assumes no technical understanding of json files. But this walkthrough will show you how to read and modify the json Key Maps.

You will first be walked through the Key Maps and how to modify them. Then you will be shown how to load your modification onto the Rev 4 after making them in a text editor on your computer.

### Computer Keyboard Key Map

Here is the default Computer Keyboard mode Key Map. 

```json
{
  "main": [
    "ESC", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "-", "=", "\b", "DELETE",
    "\t", "q", "w", "e", "r", "t", "y", "u", "i", "o", "p", "[", "]", "\n", "PAGE_UP",
    "MACRO_0", "a", "s", "d", "f", "g", "h", "j", "k", "l", ";", "'", "\\", "PAGE_DOWN",
    "LEFT_SHIFT", "`", "z", "x", "c", "v", "b", "n", "m", ",", ".", "/", "LEFT_SHIFT", "UP_ARROW", "END",
    "LEFT_CTRL", "LEFT_GUI", "LEFT_ALT", " ", "RIGHT_ALT", "LAYER", "HOME", "LEFT_ARROW", "DOWN_ARROW", "RIGHT_ARROW",
    0
  ],

  "alt": [
    "ESC", "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F0", "F11", "F12", "\b", "DELETE",
    "\t", "q", "w", "e", "r", "t", "y", "u", "i", "o", "p", "[", "]", "\n", "PRINT_SCREEN",
    "MACRO_1", "a", "s", "d", "f", "g", "h", "j", "k", "l", ";", "'", "\\", "PAGE_DOWN",
    "LEFT_SHIFT", "`", "z", "x", "c", "v", "b", "n", "m", ",", ".", "/", "LEFT_SHIFT", "UP_ARROW", "END",
    "LEFT_CTRL", "LEFT_GUI", "LEFT_ALT", " ", "RIGHT_ALT", "LAYER", "HOME", "LEFT_ARROW", "DOWN_ARROW", "RIGHT_ARROW",
    0
  ],

  "MACRO_0": "!PRESS_LEFT_ALT, KP_0, KP_1, KP_5, KP_1, !RELEASE_LEFT_ALT",
  "MACRO_1": "!PRESS_LEFT_ALT, KP_0, KP_1, KP_3, KP_3, !RELEASE_LEFT_ALT"
}
```

You can copy text above and paste it into a file to manipulate it with your text editor. The file has to be named **keyboard_usb.json**

There are two layers to this key map. One is labeled "main." That is the top level key assignments, usually reflected in whatever is printed on your key caps, but you can make some modifications.

The second layer is called "alt" for Alternative Keyboard. It allows you to have a whole second layer of assignable keys.

Below the two Keyboard layers are two Macro definitions. Macros allow multiple keystrokes to be assigned to one key. These are most useful for creating Numpad sequences when there is no Numpad. There are many useful ASCII characters writers use that can normally be produces by holding Alt and typing out a 4 digit number sequence on the Number keypad. These Macros allow you to do that because the json definitions include Key Pad outputs.

Naming convention for each keys are coming the source code linked below:
https://github.com/arduino-libraries/Keyboard/blob/master/src/Keyboard.h

#### The main layer

```json
"main": [
  "ESC", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "-", "=", "\b", "DELETE",
  "\t", "q", "w", "e", "r", "t", "y", "u", "i", "o", "p", "[", "]", "\n", "PAGE_UP",
  "MACRO_0", "a", "s", "d", "f", "g", "h", "j", "k", "l", ";", "'", "\\", "PAGE_DOWN",
  "LEFT_SHIFT", "`", "z", "x", "c", "v", "b", "n", "m", ",", ".", "/", "LEFT_SHIFT", "UP_ARROW", "END",
  "LEFT_CTRL", "LEFT_GUI", "LEFT_ALT", " ", "RIGHT_ALT", "LAYER", "HOME", "LEFT_ARROW", "DOWN_ARROW", "RIGHT_ARROW",
  0
]
```

With one exception, this should pretty much correspond to what is printed on your default keycaps and they are just typed in, key by key, in between quotes. But some keys can't be just typed in by pressing the key. So "\\b" is Backspace, "\\n" is Enter, "\\t" is TAB and "\\\\" is single Back Slash. Other keys require typing in special Words, like "LEFT_SHIFT" and "UP_ARROW"

The one exception to matching the key caps is the Caps Lock key. You can, of course, have Caps Lock by typing in "CAPS_LOCK", but the default here is to assign the Macro for Em Dash to it. If you look at Macro_0 , it is the equivalent of holding the left Alt key while typing 0151 on the nonexistent number pad.

There is also a "LAYER" key. This is the key that is held down in order to get the alternative keyboard Key Map.

You will notice there is no shift layer exposed in this Json file. You can't make modifications to this layer. However, you will get the expected output to shift. Symbols on the number keys, Capital letters and the shift layer punctuation.


#### The alt layer

```json
"alt": [
  "ESC", "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F0", "F11", "F12", "\b", "DELETE",
  "\t", "q", "w", "e", "r", "t", "y", "u", "i", "o", "p", "[", "]", "\n", "PRINT_SCREEN",
  "MACRO_1", "a", "s", "d", "f", "g", "h", "j", "k", "l", ";", "'", "\\", "PAGE_DOWN",
  "LEFT_SHIFT", "`", "z", "x", "c", "v", "b", "n", "m", ",", ".", "/", "LEFT_SHIFT", "UP_ARROW", "END",
  "LEFT_CTRL", "LEFT_GUI", "LEFT_ALT", " ", "RIGHT_ALT", "LAYER", "HOME", "LEFT_ARROW", "DOWN_ARROW", "RIGHT_ARROW",
  0
]
```

This gives you an additional layer for your computer keyboard to include some of the keys the computer is missing. You access this Key Map by holding down the Layer key. Don't feel compelled to change every key. You will see there are a lot of keys in this Key Map that haven't been changed from the main layer. 

The main change is that the F-keys are obtained by holding down the layer key and using the number keys. Importantly, Print Screen is assigned to Page Up on the alt layer. Also, the Macro assigned to Caps Lock is different, this one typing an ellipse. Simply, if you need a key that you don't have on the main and main+shift maps, and you know how to represent that key, add it to this layer. 

Make sure, when making changes, you pay attention to the syntax of the data structure - the commas, quotes, that numbers identifying a key don't have quotes, etc. And, ALWAYS, make a backup of your currently working json file before making and testing changes!

Naming convention for each keys are coming the source code linked:
https://github.com/arduino-libraries/Keyboard/blob/master/src/Keyboard.h

You can validate JSON format from a web site:
https://jsonlint.com/



### writerDeck Mode Key Map

It shouldn't be surprising that the writerDeck mode has a different Key Map. Writing on the small screen has different priorities. Plus, there are no Macros for the keyboard in writerDeck mode. 

```json
{
  "main": [
    "ESC", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "-", "=", "BACKSPACE", "DEL",
    " ", "q", "w", "e", "r", "t", "y", "u", "i", "o", "p", "[", "]", "\n", "PGUP",
    0, "a", "s", "d", "f", "g", "h", "j", "k", "l", ";", "\"", "\\", "PGDN",
    "SHIFT", "`", "z", "x", "c", "v", "b", "n", "m", ",", ".", "/", "SHIFT", "UP", "END",
    "ALT", "ALT", "ALT", " ", "ALT", "ALT", "HOME", "LEFT", "DOWN", "RIGHT",
    "MENU"
  ],

  "main-shift": [
    "ESC", "!", "@", "#", "$", "%", "^", "&", "*", "(", ")", "_", "+", "BACKSPACE", "DEL",
    " ", "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P", "{", "}", "\n", "PGUP",
    0, "A", "S", "D", "F", "G", "H", "J", "K", "L", ":", "'", "|", "PGDN",
    "SHIFT", "~", "Z", "X", "C", "V", "B", "N", "M", "<", ">", "?", "SHIFT", "UP", "END",
    "ALT", "ALT", "ALT", " ", "ALT", "ALT", "HOME", "LEFT", "DOWN", "RIGHT",
    "MENU"
  ],

  "alt": [
    "ESC", "FILE1", "FILE2", "FILE3", "FILE4", "FILE5", "FILE6", "FILE7", "FILE8", "FILE9", "FILE0", "-", "=", "BACKSPACE", "DEL",
    " ", "q", "w", "e", "r", "t", "y", "u", "i", "o", "p", "[", "]", "\n", "PGUP",
    0, "a", "s", "d", "f", "g", "h", "j", "k", "l", ";", "\"", "\\", "PGDN",
    "SHIFT", "`", "z", "x", "c", "v", "b", "n", "m", ",", ".", "/", "SHIFT", "UP", "END",
    "ALT", "ALT", "ALT", " ", "ALT", "ALT", "HOME", "LEFT", "DOWN", "RIGHT",
    "MENU"
  ],

  "alt-shift": [
    "ESC", "FILE1", "FILE2", "FILE3", "FILE4", "FILE5", "FILE6", "FILE7", "FILE8", "FILE9", "FILE0", "-", "=", "BACKSPACE", "DEL",
    " ", "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P", "{", "}", "\n", "PGUP",
    0, "A", "S", "D", "F", "G", "H", "J", "K", "L", ":", "'", "|", "PGDN",
    "SHIFT", "~", "Z", "X", "C", "V", "B", "N", "M", "<", ">", "?", "SHIFT", "UP", "END",
    "ALT", "ALT", "ALT", " ", "ALT", "ALT", "HOME", "LEFT", "DOWN", "RIGHT",
    "MENU"
  ]
}
```

You can copy it from the text above and paste it into a file to manipulate it with your text editor. The file has to be named **keyboard.json**

There are four layers to this Key Map, but really it isn't as complex as it might first appear. It is still the main Key Map and the alt Key Map. It is just that both have explicit shift layers. By and large, you will make most of your changes probably to the alt layer, just like Computer Keyboard mode, for keys that don't fit on the main or main-shift layers. Using the alt-shift layer means having to hold down both Alt and Shift to make your key stroke, which will always be more awkward than holding just Alt or just Shift. You can use that layer if you need to, but you probably won't.

There is one bit of potential terminological confusion here. For the "alt Key Map layers, "alt" is short for "alternative." But to activate the Key Map for those layers, you use the Alt key (I believe here Alt is short for Alternate, not that it matters). So to help sorting out whether a reference is to the Key Map layer or the computer key, this document will use "alt" (all lower case) for the Key Map and "Alt" (capitalized) for the computer key.



#### The main and main-shift layers

```json
"main": [
  "ESC", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "-", "=", "BACKSPACE", "DEL",
  " ", "q", "w", "e", "r", "t", "y", "u", "i", "o", "p", "[", "]", "\n", "PGUP",
  0, "a", "s", "d", "f", "g", "h", "j", "k", "l", ";", "\"", "\\", "PGDN",
  "SHIFT", "`", "z", "x", "c", "v", "b", "n", "m", ",", ".", "/", "SHIFT", "UP", "END",
  "ALT", "ALT", "ALT", " ", "ALT", "ALT", "HOME", "LEFT", "DOWN", "RIGHT",
  "MENU"
],

"main-shift": [
  "ESC", "!", "@", "#", "$", "%", "^", "&", "*", "(", ")", "_", "+", "BACKSPACE", "DEL",
  " ", "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P", "{", "}", "\n", "PGUP",
  0, "A", "S", "D", "F", "G", "H", "J", "K", "L", ":", "'", "|", "PGDN",
  "SHIFT", "~", "Z", "X", "C", "V", "B", "N", "M", "<", ">", "?", "SHIFT", "UP", "END",
  "ALT", "ALT", "ALT", " ", "ALT", "ALT", "HOME", "LEFT", "DOWN", "RIGHT",
  "MENU"
]
```

These are both typical for a QWERTY keyboard with few surprises. The TAB key has been disabled by assigning it as a second space bar. TAB would be meaningless on such a small screen. CAPS LOCK has been disabled by simply assigning it the number 0. Finally, as this editing software has no use for Ctrl, Win, or Fn keys, they have all been assigned to be Alt keys, which acts as the layer key, to switch to the alt Key Map layers. You will notice at the bottom a single key label in it's own row called "MENU." That will take you to the menu and it is assigned to the dial hold.

You will notice some slight differences in key names for those that have names rather than just being typed directly. For instance here Delete is "DEL" rather than "DELETE."


#### The alt and alt-shift layers

```json
"alt": [
  "ESC", "FILE1", "FILE2", "FILE3", "FILE4", "FILE5", "FILE6", "FILE7", "FILE8", "FILE9", "FILE0", "-", "=", "BACKSPACE", "DEL",
  " ", "q", "w", "e", "r", "t", "y", "u", "i", "o", "p", "[", "]", "\n", "PGUP",
  0, "a", "s", "d", "f", "g", "h", "j", "k", "l", ";", "\"", "\\", "PGDN",
  "SHIFT", "`", "z", "x", "c", "v", "b", "n", "m", ",", ".", "/", "SHIFT", "UP", "END",
  "ALT", "ALT", "ALT", " ", "ALT", "ALT", "HOME", "LEFT", "DOWN", "RIGHT",
  "MENU"
],

"alt-shift": [
  "ESC", "FILE1", "FILE2", "FILE3", "FILE4", "FILE5", "FILE6", "FILE7", "FILE8", "FILE9", "FILE0", "-", "=", "BACKSPACE", "DEL",
  " ", "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P", "{", "}", "\n", "PGUP",
  0, "A", "S", "D", "F", "G", "H", "J", "K", "L", ":", "'", "|", "PGDN",
  "SHIFT", "~", "Z", "X", "C", "V", "B", "N", "M", "<", ">", "?", "SHIFT", "UP", "END",
  "ALT", "ALT", "ALT", " ", "ALT", "ALT", "HOME", "LEFT", "DOWN", "RIGHT",
  "MENU"
]
```


Not much has changed here in the default layout. The main thing here is that holding the Alt key and typing a number key will instantly change which filespace you are in. If you need any special symbols or characters, I would assign them in the alt layer if possible so you don't have to hold down two keys and press a third. And while you may not need that many Alt keys, if you don't have something to replace them with, there is certainly no harm in having that many. It is recommended you have at least 2, one on the left side, one on the right as it makes it easier to hold down Alt on one side of the key board and press the key on the opposite side of the keyboard.

One last thing. You create these json files in a text editor on your computer. When you are ready to copy the file to The Rev 4 for updating, use Drive Mode and copy the file to the Rev 4 in Drive Mode. Then hit Esc to return to the menu. You need to power off the Rev 4 and power it back on again to be sure it has read the new file. That just involves disconnecting the USB cable and reconnecting it again.



## Sample Modified Key Maps

So finally, here is a sample set of Key Maps just to give you an idea of what kinds of modifications you can make. I (Hook) developed this to satisfy my own quirks, but it mostly involves a few variations on what Un Kyu Lee has in his default Key Maps.

### Sample Computer Keyboard Key Map


```json
{
  "main": [
    "ESC", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "-", "=", "\b", "DELETE",
    "\t", "q", "w", "e", "r", "t", "y", "u", "i", "o", "p", "[", "]", "\n", "PAGE_UP",
    "LEFT_SHIFT", "a", "s", "d", "f", "g", "h", "j", "k", "l", ";", "'", "\\", "PAGE_DOWN",
    "LEFT_SHIFT", "`", "z", "x", "c", "v", "b", "n", "m", ",", ".", "/", "LEFT_SHIFT", "UP_ARROW", "END",
    "LEFT_CTRL", "LAYER", "LEFT_ALT", " ", "RIGHT_ALT", "LAYER", "HOME", "LEFT_ARROW", "DOWN_ARROW", "RIGHT_ARROW",
    0
  ],

  "alt": [
    "ESC", "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10", "F11", "F12", "\b", "DELETE",
    "MACRO_0", "q", "w", "MACRO_5", "r", "t", "y", "u", "i", "o", "p", "[", "]", "\n", "PRINT_SCREEN",
    "LEFT_SHIFT", "a", "s", "d", "f", "g", "h", "j", "k", "l", ";", "MACRO_3", "MACRO_4", "PAGE_DOWN",
    "LEFT_SHIFT", "`", "z", "x", "MACRO_2", "v", "b", "n", "m", ",", "MACRO_1", "/", "LEFT_SHIFT", "UP_ARROW", "END",
    "LEFT_CTRL", "LAYER", "LEFT_ALT", " ", "RIGHT_ALT", "LAYER", "HOME", "LEFT_ARROW", "DOWN_ARROW", "RIGHT_ARROW",
    0
  ],

  "MACRO_0": "!PRESS_LEFT_ALT, KP_0, KP_1, KP_5, KP_1, !RELEASE_LEFT_ALT",
  "MACRO_1": "!PRESS_LEFT_ALT, KP_0, KP_1, KP_3, KP_3, !RELEASE_LEFT_ALT",
  "MACRO_2": "!PRESS_LEFT_ALT, KP_0, KP_1, KP_6, KP_9, !RELEASE_LEFT_ALT",
  "MACRO_3": "!PRESS_LEFT_ALT, KP_0, KP_1, KP_4, KP_7, !RELEASE_LEFT_ALT",
  "MACRO_4": "!PRESS_LEFT_ALT, KP_0, KP_1, KP_4, KP_8, !RELEASE_LEFT_ALT",
  "MACRO_5": "!PRESS_LEFT_ALT, KP_0, KP_2, KP_3, KP_3, !RELEASE_LEFT_ALT"
}
```

A few things about the changes here from the default. Notice that there are two Left Shift keys, one being the Caps Lock key. I (Hook) am kind of a clumsy typist. I have often disabled Caps Lock because I tended to hit Caps Lock when I was aiming for shift and would end up yelling at my readers for a couple of lines before I noticed. It's even easier to make that mistake when the Caps Lock key is larger than shift. So, rather than just disabling the Caps Lock key, why not also make it a shift key so that when I hit it by mistake, I still get what I want?

I also created 2 "Layer" keys, one on the right side and one on the left. This is so I can choose which layer key to use to invoke the alt Key Map layer depending on what side of the keyboard the key I need is.

Finally, most of the rest of the changes are my placements of the six Macros I created. They give me  -  (em dash), é (accented e), © (copyright), " (open smartquote), " (close smartquote), and ... (ellipse). I wanted to put them on keys that would help me remember them unless or until I create a custom key cap set. So em dash is Layer+Tab, accented e is Layer+e, copyright is Layer+c, smart open quotes is Layer+single quote and smart close quote is Layer+backslash (just because it is the key next to single quote) and ellipse is Layer+period. These are the 6 Alt-Numpad special characters I use the most.


### Sample writerDeck Mode Key Map

```json
{
  "main": [
    "ESC", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "-", "=", "BACKSPACE", "DEL",
    0, "q", "w", "e", "r", "t", "y", "u", "i", "o", "p", "[", "]", "\n", "PGUP",
    "SHIFT", "a", "s", "d", "f", "g", "h", "j", "k", "l", ";", "'", "\\", "PGDN",
    "SHIFT", "`", "z", "x", "c", "v", "b", "n", "m", ",", ".", "/", "SHIFT", "UP", "END",
    "ALT", "ALT", "ALT", " ", "ALT", "ALT", "HOME", "LEFT", "DOWN", "RIGHT",
    "MENU"
  ],

  "main-shift": [
    "ESC", "!", "@", "#", "$", "%", "^", "&", "*", "(", ")", "_", "+", "BACKSPACE", "DEL",
    0, "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P", "{", "}", "\n", "PGUP",
    "SHIFT", "A", "S", "D", "F", "G", "H", "J", "K", "L", ":", "\"", "|", "PGDN",
    "SHIFT", "~", "Z", "X", "C", "V", "B", "N", "M", "<", ">", "?", "SHIFT", "UP", "END",
    "ALT", "ALT", "ALT", " ", "ALT", "ALT", "HOME", "LEFT", "DOWN", "RIGHT",
    "MENU"
  ],

  "alt": [
    "FILE0", "FILE1", "FILE2", "FILE3", "FILE4", "FILE5", "FILE6", "FILE7", "FILE8", "FILE9", "FILE0", "-", "=", "BACKSPACE", "DEL",
    151, "q", "w", 233, "r", "t", "y", "u", "i", "o", "p", "[", "]", "\n", "PGUP",
    "SHIFT", "a", "s", "d", "f", "g", "h", "j", "k", "l", ";", 147, 148, "PGDN",
    "SHIFT", "`", "z", "x", 169, "v", "b", "n", "m", ",", 133, "/", "SHIFT", "UP", "END",
    "ALT", "ALT", "ALT", " ", "ALT", "ALT", "HOME", "LEFT", "DOWN", "RIGHT",
    "MENU"
  ],

  "alt-shift": [
    "ESC", "FILE1", "FILE2", "FILE3", "FILE4", "FILE5", "FILE6", "FILE7", "FILE8", "FILE9", "FILE0", "_", "+", "BACKSPACE", "DEL",
    0, "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P", "{", "}", "\n", "PGUP",
    0, "A", "S", "D", "F", "G", "H", "J", "K", "L", ":", "'", "|", "PGDN",
    "SHIFT", "~", "Z", "X", "C", "V", "B", "N", "M", "<", ">", "?", "SHIFT", "UP", "END",
    "ALT", "ALT", "ALT", " ", "ALT", "ALT", "HOME", "LEFT", "DOWN", "RIGHT",
    "MENU"
  ]
}
```

Given the limitations of the Rev 4 editor, there aren't a lot of changes. However, it's useful to have the same 6 special characters here as with the Computer Keyboard mode. I don't have Macros, but I can assign them by typing in the DEC values of the keys. What is wonderful is that I could assign them to exactly the same keys as I did with the computer keyboard. Consistency in Key Maps in both modes is very efficient.

The purpose here was to not tell you how you should set up your Key Maps, but to give an example of what is possible.

You can always ask questions here:

https://www.flickr.com/groups/alphasmart/discuss/72157721923133428/





