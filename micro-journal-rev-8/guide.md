# Micro Journal Rev.8: Melodica

## User Guide

Version 2.0  
Detailed guide prepared by Hook

This guide was developed using Windows. The same features should work on Linux and macOS, although some screens and menu names may look different.

## Table of Contents

- [Micro Journal Rev.8: Melodica](#micro-journal-rev8-melodica)
  - [User Guide](#user-guide)
  - [Table of Contents](#table-of-contents)
- [Part I: Quick Start Guide](#part-i-quick-start-guide)
  - [1. Install the Battery](#1-install-the-battery)
  - [2. Turn On the Micro Journal and Start Writing](#2-turn-on-the-micro-journal-and-start-writing)
  - [3. Change Files](#3-change-files)
  - [4. Transfer or Back Up Your Writing](#4-transfer-or-back-up-your-writing)
    - [Drive Mode](#drive-mode)
    - [BLE Send](#ble-send)
    - [Google Drive Sync](#google-drive-sync)
  - [5. Update the Firmware](#5-update-the-firmware)
- [Part II: Detailed User Guide](#part-ii-detailed-user-guide)
  - [Introducing the Micro Journal Rev.8](#introducing-the-micro-journal-rev8)
  - [Using the Micro Journal Rev.8](#using-the-micro-journal-rev8)
    - [The Editor on the Rev.8](#the-editor-on-the-rev8)
    - [The Menu](#the-menu)
    - [\[S\] SYNC](#s-sync)
    - [\[W\] WIFI](#w-wifi)
    - [\[L\] Language](#l-language)
    - [\[T\] BLE Keyboard](#t-ble-keyboard)
    - [\[U\] Drive Mode](#u-drive-mode)
    - [\[B\] BACK](#b-back)
  - [Customizing Key Maps](#customizing-key-maps)
  - [The Default Rev.8 Key Map](#the-default-rev8-key-map)
    - [The main and main-shift layers](#the-main-and-main-shift-layers)
    - [The alt and alt-shift layers](#the-alt-and-alt-shift-layers)
    - [Modifying the default Rev.8 Key Map (An Example)](#modifying-the-default-rev8-key-map-an-example)
  - [Computer Keyboard Key Map](#computer-keyboard-key-map)
    - [The main layer](#the-main-layer)
    - [The alt layer](#the-alt-layer)
    - [Modifying the Computer Key Map (An Example)](#modifying-the-computer-key-map-an-example)
  - [Updating the Firmware](#updating-the-firmware)
    - [Before You Begin](#before-you-begin)
    - [Web Flash Procedure](#web-flash-procedure)
- [Reporting Issues and Getting Help](#reporting-issues-and-getting-help)

---

# Part I: Quick Start Guide

## 1. Install the Battery

The battery is not included. The Micro Journal requires:

- One flat-top 18650 lithium-ion battery
- 3.7 V
- Maximum capacity of 3300 mAh
- A Torx T10H screwdriver to open the display lid

> **Warning:** Insert the battery with the correct polarity. Reversing the positive and negative ends can permanently damage the device and may cause a fire.

Charge the battery fully before first use. The red LED indicates charging, and the blue LED indicates that charging is complete. Use a USB-A to USB-C cable and charger. Some USB-C Power Delivery chargers may not work.

- [Battery buying guide](https://www.reddit.com/r/18650masterrace/comments/qp21o8/buying_18650_batteries_start_here/)
- [Compatible flat-top battery example](https://www.18650batterystore.com/products/samsung-30q)

If the `P` and `Backspace` keys do not work, first confirm that the battery is installed and that the cable is connected to the correct port. When powering the device by USB, use the upper USB-C port on the display side. The lower port is used for Drive Mode.

## 2. Turn On the Micro Journal and Start Writing

Flip the power switch on the back of the device. The editor appears almost immediately.

- Start typing to write in the current file.
- Press `Esc` to open the menu.
- Use the arrow keys, `Home`, `End`, `PgUp`, and `PgDn` to navigate.
- The status bar shows the current file number, word count, and save status.

The Micro Journal saves automatically after a short pause in typing.

## 3. Change Files

The Micro Journal provides ten file spaces, numbered 0 through 9.

- Press `Esc` to open the menu, then press a number to open that file.
- With the default key map, hold `Alt` or `Fn` and press a number to change files directly from the editor.
- In the menu, press `D` to clear the last active file. Make sure you have backed it up first.

## 4. Transfer or Back Up Your Writing

### Drive Mode

1. Connect the lower USB-C port on the display side to your computer.
2. Press `Esc` to open the menu.
3. Select **Drive Mode** or press `U`.
4. Open the Micro Journal drive on your computer and copy the `.txt` files.
5. Eject the drive from your computer, or press `Esc` on the Micro Journal and wait for the editor to return.
6. Disconnect the USB cable.

Do not disconnect the cable while files are being copied. Make sure to back up your files when editing files directly at Drive Mode. 

### BLE Send

Pair the Micro Journal with a computer or phone using **BLE Keyboard** in the menu. Open a document on the receiving device, then press the top-left and top-right keys together. With the default layout, these are `Esc` and `Del`. The current file will be typed into the open document.

### Google Drive Sync

After Google Drive Sync is configured, select **Sync** from the menu to upload the current file. See [How to Set Up Google Drive Sync](../micro-journal-rev-6-one-piece/googledrive.md).

## 5. Update the Firmware

> **Important:** A web flash erases every document and configuration file stored on the Micro Journal. Back up the entire drive before continuing.

1. Enter Drive Mode and copy all files to your computer.
2. Download the latest Rev.8 firmware from the [Micro Journal releases page](https://github.com/unkyulee/micro-journal/releases).
3. Open the [ESP Boards web flash tool](https://www.espboards.dev/tools/program/).
4. Connect the upper USB-C port on the display side to your computer.
5. Click **Connect** and choose the Micro Journal.
6. Upload `firmware_rev_8.bin`.
7. Set the flash address to `0x0`.
8. Click **Program** and wait until flashing is complete.
9. Restore your documents and configuration files through Drive Mode.

You now know everything necessary to begin writing. The rest of this guide explains each feature and the optional customization tools in more detail.

---

# Part II: Detailed User Guide

## Introducing the Micro Journal Rev.8

The Rev.8 is perhaps an evolutionary pinnacle developing a true replacement and enhancement of the Alphasmart Neo 2.  It is a digital typewriter that is both desk worthy and portable. It is instant-on. Turn on the power and you are ready to write. It has a full staggered keyboard using hot-swappable mechanical switches and keycaps. It has a monochrome RLCD screen that is readable in a wide range of lighting including direct sun. The screen is very much like e-ink without the delay, but is as easy on the eyes. You can use json files to take control of your key mapping. It has 10 directly accessible file spaces. And you are provided with three methods of moving files off your Rev.8 to your computer, including Send (Line the Neo 2). Finally, having a clamshell lid, it is very portable. 

The Rev.8 is a 75% ISO mechanical keyboard that is about the same width as the Alphasmart Neo 2 and the Rev 4, but is slightly less deep than the Rev 4. So, with the clamshell closed, the Rev.8 is a very reasonable size. The open lid puts the screen a comfortable viewing angle.	

What may surprise you is that the Rev.8 has its own internal memory and does not use an SD card. Further, you may be concerned that the internal memory is only 8 MB (not GB). But for a digital typewriter, this really is all you need. Getting rid of the memory card keeps things simple. You still have three easy methods of transferring files that we'll discuss when we get to the menu. As for the 8 MB, that's roughly 4000 pages of text. The Neo 2 had no SD cards and only enough user memory for 200 pages of text. So no, you aren't going to store several full novels on your Rev.8, but you can do some extensive drafting before transferring the files for editing.

As with all the ESP SOC Micro Journals, as well as the Neo 2, the Rev.8 is optimized for drafting. The editor is more typewriter than word processor. It is simple. You can navigate through your file and make changes (it now has infinite scroll in each file) but there are no more advanced editing features like selecting, cutting and pasting and spell check. You can manually edit anything, but this will discourage becoming so overly occupied with getting things right that the words never get out.  This is distraction free writing, and editing can be a big distraction during the rough draft process. There are no formatting options, although you can use markdown for when you export it to your computer if you like.


## Using the Micro Journal Rev.8

### The Editor on the Rev.8

Assuming you have set up your Rev.8 with a battery and the battery is charged, simply flip the switch on the back of the Rev.8 to on and the editor appears instantly on your screen. The first time, it is a blank screen (unless Un Kyu has left you a message) with the cursor at the bottom of the screen. As you start typing, lines scroll up like on a typewriter. At the bottom of the screen is a status bar that identifies which file space you are working in (0-9), how many words you have typed and whether what you have typed is saved (typing is not saved letter by letter, but every time there is a slight pause in the typing). The first time you start up the Rev.8, you will have file space 0 open.  After that, whenever you start up the Rev.8, you will be placed in whatever the last file space you were using was.

So, as previously indicated, there aren't a lot of features in this editor. You can type, you can delete or backspace and you can navigate. As for what you can type beyond what is printed on the keys, we will get to that shortly when we discuss key maps.  Navigation options are arrow keys (up/down moves one line up and down; left/right moves the cursor one character left to right), Home (send cursor to beginning of same line), End (send cursor to the end of the same line), PgUp (cursor jumps up one screens worth of lines) and PgDn (jumps down one screen).

You type, like with a typewriter. You can review lines you have written and you can make simple local edits or make notes for you to come back to when you are editing the draft after this rough typewriter draft is finished.  This is how you get words out without a lot of distractions. It is writing focused.

However there is a lot of other functionality on the Rev.8 that enables capabilities that help you do different things with your words after you have finished drafting. These options are available via the menu.


### The Menu

The menu is reached by pressing the Esc key. On the right side of the screen is a list of your numbered file spaces, 0-9, in a column.  Next to each is the number of words in each. You can type a number to jump directly to the editor with that file space loaded. At the bottom of the list of file spaces, you are given the option to press **D** to clear the file space you were using last.

On the right hand side of the menu are functions that help you manage your Rev.8 and its files. They are:

* ***SYNC***
* ***WIFI***
* ***LANGUAGE***
* ***BLE KEYBOARD***
* ***DRIVE MODE***

We will go through these one by one

### [S] SYNC

If this is the first time you have started up the Rev.8, this option may not appear. This option only appears if you have enabled WIFI. This is one of the three options for getting files off your Rev.8. Before this will work, you have to follow the instructions for setting up Google Sync and placed the necessary web app link in your config.json file. If WIFI is properly set up, and your web app is properly set up, pressing **S** should connect to WIFI, sync the last file you were working in to your Google Drive, report success, shut off WIFI and return you to your file space.

### [W] WIFI

This is where you set up your WIFI network or networks (if you use more than one, maybe at different locations).  You are asked to type in the SSID and a password. Setting up a potential network does not turn on WIFI and connect you. It only stores the information for future use. WIFI is only used for Google Drive Sync. WIFI is turned on when you request a sync to your Google drive and it is turned off again when done. WIFI does not stay on and there are no other uses for WIFI on the Rev.8.

### [L] Language

When you type **L** on the menu, you are given a list of possible languages, which will affect the key map that the Rev.8 uses. You will then be returned to the file you were working in. US is the default language, but any other language you pick will be indicated on the status bar in the editor.

### [T] BLE Keyboard

This choice actually has two functions. The first is to connect to your computer as a BT keyboard. So, when you press **T**, the Rev.8 immediately goes into pairing mode looking for a computer to pair with. While it is in that state, go to your computer and, in the BT and devices section of your settings click on "add a new device," select keyboard and let your computer scan. "Micro Journal" should show up as a found device. Click on it to complete the pairing. You should now be able to type on your computer with the Rev.8. After the first pairing the Rev.8 should just reconnect automatically when you press **T**  However, you may have to re-pair the Micro Journal the first time after a firmware upgrade.

The second function only works when the Rev.8 is paired with your computer as a keyboard. This is a second way of getting your files to your computer. It is the SEND function which works pretty much like SEND on the Neo 2. Once you have paired your Rev.8 with your computer as a keyboard, open an app on your computer where you would like text sent (word processor, text editor, anything that handles having text typed into it). Tap both the top right-most and left most keys (Esc and Del in the default key assignment) at the same time. After a short delay, you'll see your words from your last active file being typed out.  When it is finished, turn off the Rev.8.
					
### [U] Drive Mode

The purpose of this option is to allow your computer to access the internal memory of the Rev.8 so you can copy over files to the computer in order to back them up or do further editing on them.

When you press **U**, a screen asks you to connect your computer to the Rev.8 by USB. Use a cable with a USB-C plug at the Micro Journal end. Connect it to the lower USB-C port on the display side. After a short delay, a file manager window should appear and show the files stored on the Rev.8. Each file is named with its file-space number and the `.txt` extension. You can copy these files to your computer for backup or further editing. When finished, eject the drive from your computer or press `Esc` on the Rev.8. Wait for the editor to return before disconnecting the cable.

### [B] BACK
Pressing B on the Menu returns you to the last file you were working on.


The above covers everything you need to know to start using the Rev.8. Feel free to stop here and go write.

There are two more topics that are optional.  One is for customizing your key maps. This is entirely optional. The other is upgrading the firmware when an upgrade is made available. While this too is optional, or at least the timing of when you do it is, it is highly recommended. However, you don't have to rush to do it before doing some writing on your Rev.8.


## Customizing Key Maps

The Key Maps, or keyboard layout, determines what output is obtained when a key or combination of keys are pressed or held. The most obvious key map is what is printed on your keyboard keycaps. But you can change some of those assignments and The Rev.8 provides additional layers so you can have access to even more kinds of keys. In fact, there are two different keyboard maps for the Rev.8: The one it uses for its own editor and the one it uses when connected to a computer as an external keyboard. 

**Note:** There are already default keymaps used by the Rev.8 in the firmware, so you do not have to do this if you are happy with how the keyboard behaves. What is described below is an **optional** personalization of your keyboards if that is what you want.  

If you have a Rev 4, this is essentially the exact same key map method.

The key maps are determined by special text files with the extension of .json (JavaScript Object Notation). It is a way of representing structured data to the computer, in this case Key Maps. This walkthrough assumes no technical understanding of json files. But this walkthrough will show you how to read and modify the json Key Maps.

You will first be walked through the Key Maps and how to modify them. Then you will be shown how to load your modification onto the Rev.8 after making them in a text editor on your computer.


## The Default Rev.8 Key Map

This is a simple Key Map for a simple editor that simply follows what is printed on the key cap prints. There isn't an explicit json file on the Rev.8, but this shows you what a json file would look like to give you a format you can follow to customize the assignments for your personal needs.

The naming convention for each key is based on the source code linked below:
https://github.com/arduino-libraries/Keyboard/blob/master/src/Keyboard.h

```json
{
  "main": [
    "ESC", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "-", "=", "BACKSPACE", "DEL",
    " ", "q", "w", "e", "r", "t", "y", "u", "i", "o", "p", "[", "]", "\n", "PGUP",
    0, "a", "s", "d", "f", "g", "h", "j", "k", "l", ";", "\"", "\\", "PGDN",
    "SHIFT", "`", "z", "x", "c", "v", "b", "n", "m", ",", ".", "/", "SHIFT", "UP", "END",
    "ALT", "ALT", "ALT", " ", "ALT", "ALT", "HOME", "LEFT", "DOWN", "RIGHT"
  ],

  "main-shift": [
    "ESC", "!", "@", "#", "$", "%", "^", "&", "*", "(", ")", "_", "+", "BACKSPACE", "DEL",
    " ", "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P", "{", "}", "\n", "PGUP",
    0, "A", "S", "D", "F", "G", "H", "J", "K", "L", ":", "'", "|", "PGDN",
    "SHIFT", "~", "Z", "X", "C", "V", "B", "N", "M", "<", ">", "?", "SHIFT", "UP", "END",
    "ALT", "ALT", "ALT", " ", "ALT", "ALT", "HOME", "LEFT", "DOWN", "RIGHT"
  ],

  "alt": [
    "ESC", "FILE1", "FILE2", "FILE3", "FILE4", "FILE5", "FILE6", "FILE7", "FILE8", "FILE9", "FILE0", "-", "=", "BACKSPACE", "DEL",
    " ", "q", "w", "e", "r", "t", "y", "u", "i", "o", "p", "[", "]", "\n", "PGUP",
    0, "a", "s", "d", "f", "g", "h", "j", "k", "l", ";", "\"", "\\", "PGDN",
    "SHIFT", "`", "z", "x", "c", "v", "b", "n", "m", ",", ".", "/", "SHIFT", "UP", "END",
    "ALT", "ALT", "ALT", " ", "ALT", "ALT", "HOME", "LEFT", "DOWN", "RIGHT"
  ],

  "alt-shift": [
    "ESC", "FILE1", "FILE2", "FILE3", "FILE4", "FILE5", "FILE6", "FILE7", "FILE8", "FILE9", "FILE0", "-", "=", "BACKSPACE", "DEL",
    " ", "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P", "{", "}", "\n", "PGUP",
    0, "A", "S", "D", "F", "G", "H", "J", "K", "L", ":", "'", "|", "PGDN",
    "SHIFT", "~", "Z", "X", "C", "V", "B", "N", "M", "<", ">", "?", "SHIFT", "UP", "END",
    "ALT", "ALT", "ALT", " ", "ALT", "ALT", "HOME", "LEFT", "DOWN", "RIGHT"
   ]
}
```

You can copy it from the text above and paste it into a file to manipulate it with your text editor. The file has to be named **keyboard.json**

There are four layers to this Key Map, but really it isn't as complex as it might first appear. It is a main Key Map (with shift layer) and an alt Key Map (with shift layer). By and large, you will make most of your changes probably to the alt layer for keys that don't fit on the main or main-shift layers. Using the alt-shift layer means having to hold down both Alt and Shift to make your key stroke, which will always be more awkward than holding just Alt or just Shift. You can use that layer if you need to, but you probably won't.

There is one bit of potential terminological confusion here. For the "alt Key Map layers, "alt" is short for "alternative." But to activate the Key Map for those layers, you use the Alt key (I believe here Alt is short for Alternate, not that it matters). So to help sorting out whether a reference is to the Key Map layer or the computer key, this document will use "alt" (all lower case) for the Key Map and "Alt" (capitalized) for the computer key.



### The main and main-shift layers

```json
"main": [
  "ESC", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "-", "=", "BACKSPACE", "DEL",
  " ", "q", "w", "e", "r", "t", "y", "u", "i", "o", "p", "[", "]", "\n", "PGUP",
  0, "a", "s", "d", "f", "g", "h", "j", "k", "l", ";", "\"", "\\", "PGDN",
  "SHIFT", "`", "z", "x", "c", "v", "b", "n", "m", ",", ".", "/", "SHIFT", "UP", "END",
  "ALT", "ALT", "ALT", " ", "ALT", "ALT", "HOME", "LEFT", "DOWN", "RIGHT"
],

"main-shift": [
  "ESC", "!", "@", "#", "$", "%", "^", "&", "*", "(", ")", "_", "+", "BACKSPACE", "DEL",
  " ", "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P", "{", "}", "\n", "PGUP",
  0, "A", "S", "D", "F", "G", "H", "J", "K", "L", ":", "'", "|", "PGDN",
  "SHIFT", "~", "Z", "X", "C", "V", "B", "N", "M", "<", ">", "?", "SHIFT", "UP", "END",
  "ALT", "ALT", "ALT", " ", "ALT", "ALT", "HOME", "LEFT", "DOWN", "RIGHT"
]
```

These are both typical for a QWERTY keyboard with few surprises. The TAB key has been disabled by assigning it as a second space bar. TAB would be meaningless on such a small screen. CAPS LOCK has been disabled by simply assigning it the number 0. Finally, as this editing software has no use for Ctrl, Win, or Fn keys, they have all been assigned to be Alt keys, which acts as the layer key, to switch to the alt Key Map layers. 

### The alt and alt-shift layers

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

Not much has changed here in the default layout. The main thing here is that holding the Alt key and typing a number key will instantly change which file space you are in. If you need any special symbols or characters, I would assign them in the alt layer if possible so you don't have to hold down two keys and press a third. And while you may not need that many Alt keys, if you don't have something to replace them with, there is certainly no harm in having that many. It is recommended you have at least 2, one on the left side, one on the right as it makes it easier to hold down Alt on one side of the key board and press the key on the opposite side of the keyboard.

One last thing. You create these json files in a text editor on your computer. When you are ready to copy the file to The Rev.8 for updating, use Drive Mode and copy the file to the Rev.8. You should power off the Rev.8 and power it back on again to be sure it has read the new file. 


### Modifying the default Rev.8 Key Map (An Example)

Here is a sample set of Key Maps just to give you an idea of what kinds of modifications you can make. I (Hook) developed this to satisfy my own quirks, but it mostly involves a few variations on what Un Kyu Lee has in his default Key Maps.


```json
"main": 
    [
     "ESC", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "-", "=", "BACKSPACE", "DEL",
     " ", "q", "w", "e", "r", "t", "y", "u", "i", "o", "p", "[", "]", "\n", "PGUP",
     151, "a", "s", "d", "f", "g", "h", "j", "k", "l", ";", "'", "\\", "PGDN",
     "SHIFT", "`", "z", "x", "c", "v", "b", "n", "m", ",", ".", "/", "SHIFT", "UP", "END",
     "ALT", "ALT", "ALT", " ", "ALT", "ALT", "HOME", "LEFT", "DOWN", "RIGHT"   
    ],

    "main-shift": 
    [
     "ESC", "!", "@", "#", "$", "%", "^", "&", "*", "(", ")", "_", "+", "BACKSPACE", "DEL",
     " ", "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P", "{", "}", "\n", "PGUP",
     "SHIFT", "A", "S", "D", "F", "G", "H", "J", "K", "L", ":", "\"", "|", "PGDN",
     "SHIFT", "~", "Z", "X", "C", "V", "B", "N", "M", "<", ">", "?", "SHIFT", "UP", "END",
     "ALT", "ALT", "ALT", " ", "ALT", "ALT", "HOME", "LEFT", "DOWN", "RIGHT"     
    ],

    "alt": 
    [
     "ESC", "FILE1", "FILE2", "FILE3", "FILE4", "FILE5", "FILE6", "FILE7", "FILE8", "FILE9", "FILE0", "-", "=", "BACKSPACE", "DEL",
     151, "q", "w", 233, "r", "t", "y", "u", "i", "o", "p", "[", "]", "\n", "PGUP",
     "CAPS_LOCK", "a", "s", "d", "f", "g", "h", "j", "k", "l", ";", "'", 147, "PGDN",
     "SHIFT", "`", "z", "x", 169, "v", "b", "n", "m", ",", 133, "/", "SHIFT", "UP", "END",
     "ALT", "ALT", "ALT", " ", "ALT", "ALT", "HOME", "LEFT", "DOWN", "RIGHT"     
    ],

    "alt-shift": 
    [
     "ESC", "FILE1", "FILE2", "FILE3", "FILE4", "FILE5", "FILE6", "FILE7", "FILE8", "FILE9", "FILE0", "_", "+", "BACKSPACE", "DEL",
     0, "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P", "{", "}", "\n", "PGUP",
     0, "A", "S", "D", "F", "G", "H", "J", "K", "L", ":", "\"", 148, "PGDN",
     "SHIFT", "~", "Z", "X", "C", "V", "B", "N", "M", "<", ">", "?", "SHIFT", "UP", "END",
     "ALT", "ALT", "ALT", " ", "ALT", "ALT", "HOME", "LEFT", "DOWN", "RIGHT"    
    ]
} 
```

Changes are minimal. Mostly I wanted certain symbols to be available. Luckily, the Rev.8 editor Key Map offers an easy way to have symbols available using a decimal ASCII code. These are symbols you can get on a Windows computer by holding down the right Alt key and typing four numbers on the numpad.  So, on a normal Windows keyboard, if you wanted an Em Dash instead of an En Dash (hyphen), you would hold down Right Alt and type 0151 on the numpad. The Rev.8 allows you or just use the numbers directly, minus the leading 0, as key assignments in the json file. So the only change I have made to the main layer is to assign Caps Lock to produce an Em Dash by assigning the number 151 to that key in the main layer of the json file. Notice that symbol codes do not have quotes around them.

All my other changes are on the "alt layer." (I don't use the "alt-shift" layer.) I finally allow CAPS Lock to be assigned CAPS_Lock on the alt layer. I'm not going to trigger it accidentally if I have to hold down Alt first. 

The other changes are just number code special assignments like Â© assigned to the letter c and Ã© assigned to the letter e.  


## Computer Keyboard Key Map

Here is the default Key Map for when the Rev.8 is connected to your computer as a keyboard. This is even more optional than modifying the Rev * Editor keyboard. Depending on how much you use the Rev.8 as a computer keyboard, you may not want to bother with this. For instance, if you only really use this option for Send, you may not have any need to customize the keyboard. 

The default Computer Key Map looks like this:

```json
{
  "main": [
    "ESC", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "-", "=", "\b", "DELETE",
    "\t", "q", "w", "e", "r", "t", "y", "u", "i", "o", "p", "[", "]", "\n", "PAGE_UP",
    "MACRO_0", "a", "s", "d", "f", "g", "h", "j", "k", "l", ";", "'", "\\", "PAGE_DOWN",
    "LEFT_SHIFT", "`", "z", "x", "c", "v", "b", "n", "m", ",", ".", "/", "LEFT_SHIFT", "UP_ARROW", "END",
    "LEFT_CTRL", "LEFT_GUI", "LEFT_ALT", " ", "RIGHT_ALT", "LAYER", "HOME", "LEFT_ARROW", "DOWN_ARROW", "RIGHT_ARROW"
  ],

  "alt": [
    "ESC", "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F0", "F11", "F12", "\b", "DELETE",
    "\t", "q", "w", "e", "r", "t", "y", "u", "i", "o", "p", "[", "]", "\n", "PRINT_SCREEN",
    "MACRO_1", "a", "s", "d", "f", "g", "h", "j", "k", "l", ";", "'", "\\", "PAGE_DOWN",
    "LEFT_SHIFT", "`", "z", "x", "c", "v", "b", "n", "m", ",", ".", "/", "LEFT_SHIFT", "UP_ARROW", "END",
    "LEFT_CTRL", "LEFT_GUI", "LEFT_ALT", " ", "RIGHT_ALT", "LAYER", "HOME", "LEFT_ARROW", "DOWN_ARROW", "RIGHT_ARROW"
  ],

  "MACRO_0": "!PRESS_LEFT_ALT, KP_0, KP_1, KP_5, KP_1, !RELEASE_LEFT_ALT",
  "MACRO_1": "!PRESS_LEFT_ALT, KP_0, KP_1, KP_3, KP_3, !RELEASE_LEFT_ALT"
}
```

You don't need to bother creating this file if you aren't going to make any changes. If you are going to make changes, you can copy text above and paste it into a file to manipulate it with your text editor. The file has to be named **keyboard_usb.json**

There are two layers to this key map. One is labeled "main." That is the top level key assignments, usually reflected in whatever is printed on your keycaps, but you can make some modifications.

The second layer is called "alt" for Alternative Keyboard. It allows you to have a whole second layer of assignable keys.

Below the two Keyboard layers are two Macro definitions. Macros allow multiple keystrokes to be assigned to one key. These are most useful for creating Numpad sequences when there is no Numpad. Unlike the Key Map for the Rev.8, you cannot just assign numbers to the keymap directly. However, these Macros allow you to accomplish the same thing because the json definitions include Alt+Key Pad outputs. So, you can create up to 10 of these by copying the template, increment the MACRO_n naming convention and substituting the 4 KP digits to match what you would type on the num pad to get that character.



### The main layer

```json
"main": [
  "ESC", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "-", "=", "\b", "DELETE",
  "\t", "q", "w", "e", "r", "t", "y", "u", "i", "o", "p", "[", "]", "\n", "PAGE_UP",
  "MACRO_0", "a", "s", "d", "f", "g", "h", "j", "k", "l", ";", "'", "\\", "PAGE_DOWN",
  "LEFT_SHIFT", "`", "z", "x", "c", "v", "b", "n", "m", ",", ".", "/", "LEFT_SHIFT", "UP_ARROW", "END",
  "LEFT_CTRL", "LEFT_GUI", "LEFT_ALT", " ", "RIGHT_ALT", "LAYER", "HOME", "LEFT_ARROW", "DOWN_ARROW", "RIGHT_ARROW"
]
```

With one exception, this should pretty much correspond to what is printed on your default keycaps and they are just typed in, key by key, in between quotes. But some keys can't be just typed in by pressing the key. So "\\b" is Backspace, "\\n" is Enter, "\\t" is TAB and "\\" is single Back Slash. Other keys require typing in special Words, like "LEFT_SHIFT" and "UP_ARROW"

The one exception to matching the keycaps is the Caps Lock key. You can, of course, have Caps Lock by typing in "CAPS_LOCK", but the default here is to assign the Macro for Em Dash to it. If you look at Macro_0, it is the equivalent of holding the left Alt key while typing 0151 on the nonexistent number pad.

There is also a "LAYER" key. This is the key that is held down in order to get the alternative keyboard Key Map.

You will notice there is no shift layer exposed in this Json file. You can't make modifications to this layer. However, you will get the expected output to shift. Symbols on the number keys, Capital letters and the shift layer punctuation.


### The alt layer

```json
"alt": [
  "ESC", "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F0", "F11", "F12", "\b", "DELETE",
  "\t", "q", "w", "e", "r", "t", "y", "u", "i", "o", "p", "[", "]", "\n", "PRINT_SCREEN",
  "MACRO_1", "a", "s", "d", "f", "g", "h", "j", "k", "l", ";", "'", "\\", "PAGE_DOWN",
  "LEFT_SHIFT", "`", "z", "x", "c", "v", "b", "n", "m", ",", ".", "/", "LEFT_SHIFT", "UP_ARROW", "END",
  "LEFT_CTRL", "LEFT_GUI", "LEFT_ALT", " ", "RIGHT_ALT", "LAYER", "HOME", "LEFT_ARROW", "DOWN_ARROW", "RIGHT_ARROW"
]
```

This gives you an additional layer for your computer keyboard to include some of the keys you might need for the computer that are never used on the Rev.8. You access this Key Map by holding down the Layer key. Don't feel compelled to change every key. You will see there are a lot of keys in this Key Map that haven't been changed from the main layer. 

The main change is that the F-keys are obtained by holding down the layer key and using the number keys. Importantly, Print Screen is assigned to Page Up on the alt layer. Also, the Macro assigned to Caps Lock is different, this one typing an ellipse. Simply, if you need a key that you don't have on the main and main+shift maps, and you know how to represent that key using a Macro, add it to this layer. 

Make sure, when making changes, you pay attention to the syntax of the data structure - the commas, quotes, that numbers identifying a key don't have quotes, etc. And, ALWAYS, make a backup of your currently working json file before making and testing changes!

You can validate JSON format from a web site:
https://jsonlint.com/


### Modifying the Computer Key Map (An Example)

Here is an example of the mods I (Hook) made to the Key Map the Rev.8 uses when I am connected to my computer. This is just an example, based on my needs. YMMV, but this will give you an idea of what you can do.

```json
{
  "main": [
    "ESC", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "-", "=", "\b", "DELETE",
    "\t", "q", "w", "e", "r", "t", "y", "u", "i", "o", "p", "[", "]", "\n", "PAGE_UP",
    "LEFT_SHIFT", "a", "s", "d", "f", "g", "h", "j", "k", "l", ";", "'", "\\", "PAGE_DOWN",
    "LEFT_SHIFT", "`", "z", "x", "c", "v", "b", "n", "m", ",", ".", "/", "LEFT_SHIFT", "UP_ARROW", "END",
    "LEFT_CTRL", "LAYER", "LEFT_ALT", " ", "RIGHT_ALT", "LAYER", "HOME", "LEFT_ARROW", "DOWN_ARROW", "RIGHT_ARROW"
  ],

  "alt": [
    "ESC", "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10", "F11", "F12", "\b", "DELETE",
    "MACRO_0", "q", "w", "MACRO_5", "r", "t", "y", "u", "i", "o", "p", "[", "]", "\n", "PRINT_SCREEN",
    "LEFT_SHIFT", "a", "s", "d", "f", "g", "h", "j", "k", "l", ";", "MACRO_3", "MACRO_4", "PAGE_DOWN",
    "LEFT_SHIFT", "`", "z", "x", "MACRO_2", "v", "b", "n", "m", ",", "MACRO_1", "/", "LEFT_SHIFT", "UP_ARROW", "END",
    "LEFT_CTRL", "LAYER", "LEFT_ALT", " ", "RIGHT_ALT", "LAYER", "HOME", "LEFT_ARROW", "DOWN_ARROW", "RIGHT_ARROW"
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

Finally, most of the rest of the changes are my placements of the six Macros I created. They give me  -  (em dash), Ã© (accented e), Â© (copyright), " (open smartquote), " (close smartquote), and ... (ellipse). I wanted to put them on keys that would help me remember them unless or until I create a custom key cap set. So em dash is Layer+Tab, accented e is Layer+e, copyright is Layer+c, smart open quotes is Layer+single quote and smart close quote is Layer+backslash (just because it is the key next to single quote) and ellipse is Layer+period. These are the 6 Alt-Numpad special characters I use the most.


Given the limitations of the Rev.8 editor, there aren't a lot of changes. However, it's useful to have the same special characters assigned to the same keys in both Key Map modes. 

The purpose here was to not tell you how you should set up your Key Maps, but to give an example of what is possible.

You can always ask questions here:

https://www.flickr.com/groups/alphasmart/discuss/72157721925271377/


## Updating the Firmware

Updating firmware on the Rev.8 requires using a web flash tool in your browser. The instructions can be found here:

https://github.com/unkyulee/micro-journal/releases/

This is where you go to find the firmware asset you download to your computer. Scroll down until you find the Web Flash Update Instructions. 

Here are the instructions you will find there:

### Before You Begin

The web flash process will **erase all files stored on the device.** Back up your files before continuing:

    Open Drive Mode on the Micro Journal.
    Connect the device to your computer.
    Copy all documents and configuration files to a safe location.
    Confirm that the backup is complete before flashing.

### Web Flash Procedure

   1. Open the web flash tool:

    https://www.espboards.dev/tools/program/

   2. Connect the ESP32 USB port to your computer. 
           When the screen is open, use the upper USB port.

   3. Click Connect in the web flash tool.

   4. Select the Micro Journal device from the connection window.

   5. Upload the firmware to the web site

   6. Set the flash address to: 0x0

   7. Click Program.

   8. Wait for the firmware installation to finish before disconnecting the device.

After flashing, restore your documents and configuration files from the backup.

It is useful to point out one point more specifically.  You are asked to use Drive Mode to back up all the files on the Rev.8 to your computer **before starting** the Web Flash Update.  This is not just a backup in case something goes wrong. The Web Flash Update will wipe everything on your Rev.8. That is how it works.  Then you copy all your files back using Drive Mode to restore what you had. If everything isn't backed up, you will lose both writing and settings.



---

# Reporting Issues and Getting Help

If you encounter a problem with the Micro Journal Rev 2.1, the preferred way to report it is through the project GitHub issues page. This helps track problems, share solutions, and improve future revisions.

To report an issue:

1. Visit the Micro Journal issues page:
   [https://github.com/unkyulee/micro-journal/issues](https://github.com/unkyulee/micro-journal/issues)

2. Check existing issues to see if your problem has already been reported. If it has, you can add additional details or confirm that you are experiencing the same behavior.

3. If your issue is new, select **New issue** and provide as much detail as possible, including:

   * A clear description of the problem
   * What you were doing when the issue occurred
   * Any error messages shown on screen
   * Your power setup, including charger type, cable, and whether batteries are installed
   * Photos or videos, if they help illustrate the issue

Clear and detailed reports make it much easier to diagnose problems and respond effectively.

If you are unfamiliar with GitHub, you may need to create a free GitHub account before submitting an issue. Once submitted, you can return to the issue page to follow updates or respond to questions.

