# 📘 Micro Journal Rev.6 — User Manual

Version 1.1  
Prepared by Hook 
Edited by Un Kyu Lee

---

## Table of Contents
1. Introduction
2. First Steps & Setup
   - Battery Installation
   - SD Card Preparation
   - Firmware Update
   - Strap Installation
   - Google Drive Sync
3. Using the Micro Journal Rev.6
   - The Editor
   - File Spaces
   - Menu System
   - Syncing with Google Drive
   - WiFi Setup
   - Bluetooth Keyboard Mode
4. Start & Sleep Animations
5. Keyboard Layout Customization
   - Keymap JSON Explained
   - USB Keyboard Mode Layout
   - Example Custom Layouts (Hook’s Keymaps)
6. Troubleshooting & Maintenance
7. Appendix & Resources

---

## 1. Introduction

The Rev 6 is one of a line of Micro Journal Writer Decks aimed at (my words) *writing focused* drafting. That is, they are digital typewriters that help you focus on getting the words out when you are drafting. That is certainly what the Rev 6 is optimized for. It can do other things. You can edit on it, though you probably should export the text to a more capable computer (which the Rev 6 has the means to do) for the heavier lifting type of editing like moving text around and rewriting. However, all in all, the Rev 6 stands out in the Micro Journal line as the simplest and most budget friendly replacement for the Neo 2. 

<img src="./images/005.png">

The software is simple, but capable, and the Rev 6's unibody, light weight design with mechanical keyboard and small screen makes it perfect for carrying out with you in a bag. It even has a strap you can use to carry it without a bag. In addition, it has the "Send" capability if you don't want to sync to Google's cloud. However, thanks to Bluetooth (BT) built in, which the Neo 2 never had, it doesn't need a usb cord to use "send" (it can also act as a BT keyboard for your computer. We will get to all that in this guide).

The one real difference is that it uses an ortholinear keyboard, meaning it has fewer keys (48) and the keys are arranged in a matrix pattern rather than the mmore common stagggered pattern. If you have never used this kind of key arrangement, it will take a short while to get used to it. However, it doesn't take long. What is necessary is having layers so you can end up with all the keys you want (using the layers, you can, in theory, have 196 different key assignments. But you won't want to use anywhere near that many).

---

## 2. First Steps & Setup

### Battery Installation
- (not included) Requires **one 18650 Li-ion 3.7V battery**.  
- Capacity should not exceed **3300 mAh**.  
- Insert with correct polarity (+ and –).  
- Charge fully before first use (red LED = charging, blue LED = charged).  
- Use USB-A to USB-C chargers (PD chargers may not work).  
- [Battery Buying Guide](https://www.reddit.com/r/18650masterrace/comments/qp21o8/buying_18650_batteries_start_here/?share_id=RTqe02Hqv2l_VwJTwoIi1&utm_content=2&utm_medium=android_app&utm_name=androidcss&utm_source=share&utm_term=1)
- [This Battery Works, Flat-top (US)](https://www.amazon.com/gp/aw/d/B0CPBGDP1H)

⚠️ Wrong polarity can cause **irreparable damage or fire**.

⚠️ You will also need TORX T10H hex screw driver in order to open up the display lid in order to place the battery

⚠️ When the 'P' and 'Backspace' Keys Aren't Working. This issue usually occurs if the battery hasn't been installed or if the USB cable is plugged into the wrong port. Make sure not to use the USB-C port on the lower left side, which is closer to the keyboard. There are two USB-C ports on the left side of the device; always use the upper USB-C port to power the device properly.


### SD Card Preparation
- (not included) Use ≤32 GB SD card, formatted **FAT32**.  
- Insert carefully into the deep slot to avoid dropping inside enclosure.  
- Larger cards require partitioning.  


### Firmware Update
1. Download latest firmware from [GitHub Releases](https://github.com/unkyulee/micro-journal/releases).  
2. Copy `firmware_rev_6.bin` to SD card.  
3. Reboot device; it will auto-update.  


### Strap Installation
![strap](./images/strap_001.png)
- Place strap between knob and ring.  
- Avoid overtightening.  
- Replacement STL files available on GitHub.


### Ports and Buttons

![Diagram](./images/diagram.png)


### Google Sync Setup

If you have already set up Google Sync on another Micro Journal, you can reuse the same URL stored in the `config.json` file on your SD card. Otherwise, follow the steps below to configure Google Sync for the first time.

### 1. Connect to Google Drive

Open [Google Drive](https://drive.google.com) in your browser.

### 2. Create a Folder

Create a new folder named **uJournal**.  
This will be the destination for your synced files.

![drive](./images/drive_001.png)

![drive](./images/drive_002.png)

### 3. Create a Google Apps Script

Navigate inside the newly created **uJournal** folder. From within the folder, create a new **Google Apps Script** project.

![drive](./images/drive_003.png)


### 4. Copy the Sync Script
1. Open the latest sync script here:  
   [Sync Script (GitHub)](https://raw.githubusercontent.com/unkyulee/micro-journal/main/micro-journal-rev-4-esp32/install/google/sync.js)  
2. Copy the entire code and paste it into your new Apps Script project, replacing any existing content.  
3. Give the project a recognizable name (e.g., *uJournal Sync*).

![drive](./images/drive_004.png)

### 5. Deploy the Script
1. Click **Deploy** → **New Deployment**.  
2. Choose **Web app** as the deployment type.  
3. Configure as follows:  
   - **Execute as:** *Me* (this allows the script to access your Google Drive).  
   - **Who has access:** *Anyone* (only those with the URL can access, so keep it private).  
 
 ![drive](./images/drive_005.png)
![drive](./images/drive_006.png)
![drive](./images/drive_007.png)

### 7. Authorize the Script
1. Press **Deploy**.  
2. Sign in with your Google account and authorize the script.  
   - You may see a warning that the script is unverified. Since this script runs only in your own Google Drive, it is safe to proceed.  
3. Confirm access when prompted.

![drive](./images/drive_008.png)
![drive](./images/drive_009.png)
![drive](./images/drive_010.png)


### 8. Copy the Web App URL
After deployment, you will be given a **Web App URL**. Copy this link — it will be used by your Micro Journal Rev.6 to sync files.

![drive](./images/drive_011.png)

### 9. Configure the SD Card
1. Insert the SD card into your computer.  
2. Open (or create) a file named `config.json`.  
3. Replace its contents with the following template, inserting your Web App URL in place of the placeholder:

```json
{
  "sync": {
    "url": "PASTE_YOUR_WEB_APP_URL_HERE"
  }
}
```

⚠️ Be careful not to remove or add extra commas, brackets, or quotes. If the file becomes corrupted, delete its contents and paste the template again.

4. Save the file, safely eject the SD card, and reinsert it into the Micro Journal Rev.6.

### 10. Test the Sync
Ensure WiFi is configured before attempting sync.  

1. On the device, press **MENU**.  
2. Press **S** to initiate sync.  

**Note:** The ESP32 only supports **2.4 GHz WiFi**. It will not connect to 5 GHz networks.


---

## 3. Using the Micro Journal Rev.6

### The Editor
For right now, we are only considering the Rev 6's editor and it's very simple feature set. We will save other features like wifi annd BT connectivity for the next section when we look at the menu. The Editor is the core functionality of the Rev 6 that requires no external connections. It is a digital typewriter to help you get your words out with no distractions.

The editor is very simple. It types characters. It has a 320 x 240 pixel color LCD screen, which, with the font size used gives you 11 rows with aboiut 25 characters across. The text is quite sharp, clear and readable assuming good vision (including good vision correction). There is only one thing this editor does. It types words in plain text format, saves them as you go and allows you to navigate through the text with arrow keys if you want to check somthing, delete or backspace. There is no copy and paste, no spell check, no paragraph formatting (although you can do that with markdown if you wish) and no AI. Best of all, there is no need to be connected to the internet or BT to use this editor and save your texts. If you are distracted, it won't be because of anything on the Rev 6.


###  File Spaces

The editor has 10 filespaces, labeled 0-9. These filespaces are where you do your writing.This is very much like the Neo 2 except you don't have dedicated keys to open the different file spacees. Instead you either use the menu to select filespaces or keyboard shortcuts.

When you first turn on the Rev 6, you are instantly placed in the first filespace, numbered 0. The screen is blank except for a status area at the very bottom of the screen. In the status area you will see (left to right) the filespace number, the word count and, to the far right a green dot. When you start typing, the red dot will change to red, meaning there is unsaved typing, and the word count will start updating. When writing, words appear at the bottom and are pushed up, a typewriter metaphor. The text word-wraps with Enter creating a new paragraph. 

The Rev 6 doesn't save letter-by-letter, but will save when you pause your typing. You will see the red dot turn to green whenever you pause. You are safest if you wait (it isn't a long one) until the dot turns green before turning off the Rev 6. And that's it. You type. You can use arrow keys to navigate to review or make small local fixes. And you can check your word count. By the way, subsequent to this first session, the Rev 6 will start up in whatever file space you were working in last. 

Be aware that the buffer that maintains the text on your screen, allowing you to navigate back to review the text, is limited. When the buffer limit is exceeded, text at the top of the buffer (which will be the earliest text you typed) is lost in the sense that you can't navigate to review it. However, the text is still in your file on the SD card. Nothing should ever be lost.

So, you have turned on the Rev 6 and written some text. Now, what can you do with that text? Do you want to edit it on your computer? How do you get it there? Can you sync directly or to a cloud? What settings are available to change your experience of the editor? For these questions or more, we turn to the menu system. 




### Menu System

On the default keyboard layout, the top row left-most key is the menu key, whatever that key is labeled on the key cap. Most often it will be the Esc key.

The Menu has several types of things. The top of the screen shows what firmware version you have. On the right hand side of the menu screeen is a list of the 10 file spaces.  You can change filespace here by simply typing a number. You can also delete the contents of a file space here by typing "D" on this screen. It will delete the contents of the last file space you were working in (you'll see the number of the filespace to be deleted in parentheses next to the Clear file item.)

On the left side of the menu are are various settings and functions that affect the editor or allow you to backup you text files to other devices or the cloud. Each one is engaged by typing the singlle letter in brackets next to them.They are:

Access via **Menu/Esc key**. Options:  
- `[S] Sync` → Upload to Google Drive.  
- `[W] WiFi` → Configure SSID/password.  
- `[T] BLE Keyboard` → Use as Bluetooth keyboard.  
- `[G] Background Color` → Customize editor background.  
- `[C] Font Color` → Customize text color.  

### Syncing with Google Drive
- Requires Google Apps Script setup.  
- Add sync URL to `config.json` on SD card.  
- Press `[S]` in menu to sync.  
- 
This requires WIFI (the next menu item) to be set up first and it requires the Google Apps Script you set up in the Quickstart guide (which  is done on your computer, and then the app link is added to the config.json file on your Rev 6). If everything is set up correctly, Typing "S" on this menu will send whatever was in the file you were just working in to Google Drive. If you did this for another Micro Journal already you can reuse the same link withoout going through the whole setup again.

### WiFi Setup
- Supports **2.4 GHz only**.  
- Up to 5 profiles.  
- Enter SSID and password via menu.  

Typing "W" on the menu screen will take you to the Wifi settings screen. You only should need to do this once, though you can have up to 5 wifi profiles if you take the Rev 6 to different locations. Type the number of the profile you want to set up or edit. You will be asked for an SSD and a password for that SSD. The software will then test the connection. If it succeeds, you are done.  Wifi doesn't stay on, there's no reason for it to do so.  It is only used for syncing to Google Drive. Running Sync will turn on wifi, sync and turn off Wifi.


### Bluetooth Keyboard Mode
- Acts as BLE keyboard for PC/tablet/phone.  
- “Send” function types out stored file into active window.  
- Trigger by holding first + last key of top row.  

This has two functions. "BLE" stands for Bluetooth Low Energy. It makes a limited range connection to either a computer, tablet or phone to allow the Rev 6 to be used as a keyboard for any of those 3 devices. The first time with any device you will have to do the pairing, but after that the connection should be automatic once you invoke it on the Rev 6 by typing "T".

The second function is that, while connected to your other device as a keyboard, you can also "Send" the text of the file you last worked on to your computer or phone. This is just like the "Send" function on an Alphasmart Neo. Make sure the active window on your computer or phone is where you want the text sent, such as your word processor. Then hold down the first and last key of the top row simultaneously. The Rev 6 will begin typing out the file into whatever text handling program you have prepared. Keep holding the keys until all the text has been sent as letting go will stop the send. If you hold down the two keys again, the send will restart from the beginning.

---

## 4. Start & Sleep Animations

- Place `wakeup.gif` and `sleep.gif` on SD card.  
- Specs: **320×240 px, ≤1 MB**.  

If you want to have the Startup and Sleep animation. You can place the gif files  in the SD card. Filename should be exactly the same in order to have the effect. 

Use the following web site to create your own animation files.
- https://ezgif.com/


---

## 5. Keyboard Layout Customization

- Controlled by `keyboard.json` (editor) and `keyboard_usb.json` (USB/BLE).  

The Key Maps, or keyboard layout, determines what output is obtained when a key or combination of keys are pressed or held. The most obvious key map is what is printed on your keyboard key caps. But you can change some of those assignments and The Rev 6 provides additional layers so you can have access to even more kinds of keys. In fact, there are two different keyboard maps for the Rev 6: the Key Map for the Rev 6's editor and a Key Map for using the Rev 6 as a BLE Computer Keyboard.

The key maps are determined by special text files with the extension of .json (JavaScript Object Notation). It is a way of representing structured data to the computer, in this case Key Maps. The firmware has an internally determined key map and if you are happy with that, what follows will simply show you that keymap. But if you want to  make changes, you will need to copy the default json Key Map settings into a file and make your alterations to that file. Then copy these files (one for the Rev 6's editor and one for the  Rev 6 as BLE keyboard) to the SD card.

This walkthrough assumes no technical understanding of json files. But this walkthrough will show you how to read and modify the json Key Maps.

You will first be stepped through the Key Maps and, then, how to modify them. Then you will be shown how to load your modification onto the Rev 6 after making them in a text editor on your computer.



### Rev 6 writerDeck Mode Default Key Mapping

Here is the default layout for the Rev 6's Key Map. 

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

There are three layers to this Key Map,each with a shift version (meaning what you get on each layer when you hold down shift). It really isn't as complex as it might first appear. You probably will never need all six possibilities for key assignments and, in fact, as will be explained momentarily, you start out with access to only two layers and their shift versions. It does offer a great deal of flexibility. However, keep in mind the simplicity of the editor. More will be said about how many layers you want in the discussion of customizing the layout when that comes up.



| Special Key Written AS | Description                               |
| ---------------------- | ----------------------------------------- |
| ESC                    | Functions as entering the MENU screen     |
| SHIFT                  | This is a SHIFT key                       |
| LOWER                  | Hidden layer activation key               |
| RAISE                  | Hidden layer activation key               |
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


#### The main and main-shift layer

```json
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
```

These are both typical for a Ortholinear QWERTY keyboard with few surprises. The big trade-off that has been made is that, since you are down to 48 keys, the number keys and the symbols that appear above the number keys have been moved to another layer. It makes sense for a Writer Deck where the key focus is on words and punctuation. And it isn't hard to adjust to it (we'll get to the specifics when we do the second layer).

Most of this layer has the standard Qwerty layout of letters and punctuation. Some of the punctuation is in non-standard placements due to limited number of keys and not needing keys like Ctrl and Alt for the Rev 6's simple editor. And the key at the rightmost position, usually the Esc key, is always the menu button. 

Most keys in the json file are defined by typing the character between quote marks followed by a comma. Some require special keywords typed between the quotes, and some use special symbols, so Enter here is *\n* typed between quotes. Space is just a space typed on the space bar between quotes. Note that by default, two single keys at the center of the bottom row are assigned as Space. Ortholinear keyboards often don't have spacebars.  By default, the directional navigation keys are arranged linearly in the bottom row rather than spacially as a D-pad, and holding shift gets you bigger jumps. All of it should be understandable to you. 

There is one key keyword that is new. That is the "LOWER" key.  That key, when held down, is what gets you the second layer key assigments.


#### The lower and lower-shift layer

```json
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
```
Here is where the numbers and number symbols are. They are in the correct position in the top row, they just aren't printed on the keys unless you create custom Key Caps.  You get the numbers by holding the "lower" key and typing the appropiate top row key. So lower+q=1, lower+w=2. etc.

The symbols usually obtained by holding shift and a number key is here accomplished by holding  shift  while also holding the lower key and typing the appropriate top row key. So shiift+lower+q=!, shift+lower+w=@, etc. These are the only key mappings that aren't already available on the main layer. So numbers and their symbols are really the only reason for using the lower layer in the default Key Map. No other changes were made because no other changes were needed. So this layer was created by simply copying the main layer key maps and making just the changes needed and leaving the rest as-is.  

Now let's look at the third layer.


#### The raise and raise-shift layer

You may have noticed something odd here. To get to the lower layer, there is a "LOWER" key you can hold.  But what is this about a raise layer? Where is the "RAISE" key? Well, it hasn't been assigned yet. In part, that is just to keep the default setup simple while you are learning the Rev 6. You can add a "Raise" key to the main Key Map any time you like and use it the same way you used the "LOWER" key to add another layer.


```json
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
        ],
```

The only thing new here are shortcuts to the 10 filespaces arranged on the top row so   they correspond to the 10 number keys. So if you want to switch to a different file space directly, without going through the menu, just add a "RAISE" key and you can hold raise and type the appropriate key and you will switch. That is the only change made and it is redundantly repeated on the shift part of the layer.

That is the default Key Map the Rev 6 uses for it's editor. However, when you connect the Rev 6 using BLE to act as a keyboard for a computer or phone, it uses a different computer keybord Key Map.



### Rev 6 Default BLE Keyboard Key Mapping

Here is the default Key Map that becomes available when you make a BLE connection to a computer.

```json
{
    "main": 
    [
     "ESC", "q", "w", "e", "r", "t", "y", "u", "i", "o", "p", "\b",
     "\t", "a", "s", "d", "f", "g", "h", "j", "k", "l", ";", "\"",
     "LEFT_SHIFT", "z", "x", "c", "v", "b", "n", "m", ",", ".", "/", "RIGHT_SHIFT",
     "LEFT_CTRL", "LEFT_GUI", "LEFT_ALT", "RAISE", "LOWER", " ", " ", "LEFT_ARROW", "DOWN_ARROW", "UP_ARROW", "RIGHT_ARROW", "/n"
    ],

    "lower": 
    [
     "ESC", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "DELETE",
     "`", "a", "s", "d", "f", "g", "h", "j", "-", "=", "[", "]",
     "LEFT_SHIFT", "z", "x", "c", "v", "b", "n", "m", ",", ".", "/", "RIGHT_SHIFT",
     "LEFT_CTRL", "LEFT_GUI", "LEFT_ALT", "RAISE", "LOWER", " ", " ", "HOME", "PAGE_DOWN", "PAGE_UP", "END", "/n"
    ],

    "raise": 
    [
     "ESC", "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10", "DELETE",
     "\\", "a", "s", "d", "f", "g", "h", "j", "-", "=", "[", "]",
     "LEFT_SHIFT", "z", "x", "c", "v", "b", "n", "m", ",", ".", "/", "PRINT_SCREEN",
     "LEFT_CTRL", "LEFT_GUI", "LEFT_ALT", "RAISE", "LOWER", " ", " ", "HOME", "PAGE_DOWN", "PAGE_UP", "END", ""
    ]
}
```

Keywords used in the template follows the names from the following link
- https://github.com/arduino-libraries/Keyboard/blob/master/src/Keyboard.h

This is a little different from the Rev 6's editing keyboard, although a lot is also much the same. There are three layers as before, but there is no shift portion for the layers shown.  This is because the shift behavior is handled at the PC or the phone. If the assigned key has a natural shift character, that is what you will get when you hold down shift.

For example, as you would expect, on the main layer, if you hold shift and type the letter "q" you will get "Q". If you hold the key assigned as "LOWER" and type "q" you will get "1". And if you hold the "LOWER" key and also hold shift and type "q" yoiu will get "!".  That's why the symbols for the number keys don't appear on the map. They are the already determined shift characters for the numbers, you can't change them. In the case of the F-Keys on the raise layer, holding the "RAISE" key and typing "q" will get you F1, but holding shift at the same time will get you nothing as there is no natural shift character or function for the F-Keys.

So you can make any changes you want to the three non-shift layers, but what happens when you hold shift is determined by what character or function is assigned to the key.

Notice, again, that the only changes made to the lower and raise layers are to add what  is not available on ther main layer. If a key isn't being changed, the main layer key assignment is just left as is.


### Modifying the Rev 6 Key Maps.

If you are more or less happy with the default settings for your key maps, you are set. You don't have to do anything. However, if you would like to make personal tweaks, copy the two Key Map Json arrays as shown above and paste them into two files. The Key Map for the Rev 6 editor should be copied ito a text file called *keyboard.json* and the Key Map for the BLE Keyboard should be copied to a text file called *keyboard_usb.json* The files must have those names.

Make the changes you want to each file in a plain text editor.  It may help to review the instructions at https://github.com/unkyulee/micro-journal/blob/main/micro-journal-rev-6-one-piece/keymap.md

When you are done, save the files. Remove the sd card from your Rev 6 and copy these files onto that card. Put the card back in the Rev 6. If all is well, when you start up the Rev 6, the keyboard should reflect your changes. Remember, if things mess up, you can always delete these two json files from the Rev 6 which will return you to the defaults while you trouble shoot what went wrong.

I (Hook) will illustrate below the changes I made for my Rev 6 to try to illustrate what kinds of considerations might lead you to want to change the default layouts and to give you examples of changes.


#### Hook's Rev 6 wrtierDeck Key Map

```json
{
    "main": 
    [
     "", "q", "w", "e", "r", "t", "y", "u", "i", "o", "p", "BACKSPACE",
     "-", "a", "s", "d", "f", "g", "h", "j", "k", "l", ";", "'",
     "SHIFT", "z", "x", "c", "v", "b", "n", "m", ",", ".", "UP", "\n",
     "=", "[", "]", "LOWER", "", " ", "", "RAISE", "/", "LEFT", "DOWN", "RIGHT"
    ],

    "main-shift": 
    [
     "MENU", "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P", "DEL",
     "_", "A", "S", "D", "F", "G", "H", "J", "K", "L", ":", "\"",
     "SHIFT", "Z", "X", "C", "V", "B", "N", "M", "<", ">", "PGUP", "\n",
     "+", "{", "}", "LOWER", "", " ", "", "RAISE", "?", "HOME", "PGDN", "END"
    ],

    "lower": 
    [
     "MENU", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "BACKSPACE",
     151, "", "", "", "", "", "", "", "", "", "", "",
     "SHIFT", "", "", "", "", "", "", "", "", "", "UP", "SHIFT",
     "\\", "", "", "", "", " ", "", "", "", "LEFT", "DOWN", "RIGHT"
    ],

    "lower-shift": 
    [
     "MENU", 1001, 1002, 1003, 1004, 1005, 1006, 1007, 1008, 1009, 1000, "DEL",
     "`", "", "", "", "", "", "", "", "", "", "", "",
     "SHIFT", "", "", "", "", "", "", "", "", "", "", "SHIFT",
     "", "", "", "LOWER", "", " ", "", "", "", "", "", ""
    ],

    "raise": 
    [
     "MENU", "!", "@", "#", "$", "%", "^", "&", "*", "(", ")", "DEL",
     "~", "", "", "", "", "", "", "", "", "", "", "",
     "SHIFT", "", "", "", "", "", "", "", "", "", "UP", "SHIFT",
     "|", "", "", "", "", " ", "", "", "", "LEFT", "DOWN", "RIGHT"
    ],

    "raise-shift": 
    [
     "MENU", 1001, 1002, 1003, 1004, 1005, 1006, 1007, 1008, 1009, 1000, "DEL",
     "", "", "", "", "", "", "", "", "", "", "", "",
     "SHIFT", "", "", "", "", "", "", "", "", "", "", "SHIFT",
     "", "", "", "", "", " ", "", "", "", "", "", ""
    ]
}
```

A lot of my changes on my main layer are driven by two big changes in key layout.  First, I like having a real spacebar. Not full size, but not multiple single keys. I find that a 3 key length spacebar is just right. To achieve that, I remove the first and third switch and balance a three key length spacebar on the middle switch. I do this because no three key length spacebar aligns the sockets right to hit all three switches. Luckily the space bar is still short enough to not teeter-totter in this situation.  Notice that those two removed switches are represented by quotes with nothing inside, which means nothing will happen if that key is pressed.

Second, I like my arrow keys arranged as a D-pad, so that the key positions correspond to their directions. I also am adding a "RAISE" key.  So, I'm reducing the number of keys available and I'm shuffling things around. I moved slash-qustion mark to the bottom row. I moved Enter up to the third row (just felt more natural to me). I switched backslash-pipe and dash-underline and finally, I eliminated the Menu key on the main non-shift layer. This means I have to hold shift to get the menu key to work. Why? Same reason I never keep Caps Lock on a keyboard. I'm a clumsy typist and it is too easy to hit Menu in the middle of writing and suddenly I'm dumped onto the menu screen.  I prefer not to have that happen.

Moving on to the lower and raise layers, the first thing you will notice is that I have turned off all keys that are redundant to the main layer. I have done this because it just makes the map for these layers much more readable. I am only putting what I don't have on the main layer on these layers. 

So, on the non-shifted lower layer, the most important change is a key that is a number, with no quotes. You can use 3 digit ASCII codes to get special characters. So 151 is an Em-Dash, which I make available on the dash-underline key.  I may add others, but I wanted you to see an example of this. As with the default, I have the number keys on the top row when you hold the "LOWER" key.

However, I have moved the number key symbols from the lower-shift key map to the raise key map.  Why? because I don't want to have to hold down 2 keys to get the symbols.  Lower+ top row gets me numbers.  Raise + top row gets me symbols. Easy.



---
Got it — I’ll keep all your text intact but reorganize it into a **clearer, more structured layout** with headings, subheadings, and bullet points where appropriate. This way, it reads smoothly in the manual without losing any of the original content.

---

## 6. Troubleshooting & Maintenance

### General Information
The **Micro Journal Rev.6** is built with components that you can easily find on Amazon. If some of the components are not working properly, or become worn out, you should be able to find replacements from the market quite easily. For detailed technical reference, wiring diagrams, and build steps, consult the official build guide:  

 - [Build Guide](https://github.com/unkyulee/micro-journal/blob/main/micro-journal-rev-6-one-piece/build-guide.md)

This document should provide enough information to help you identify hardware issues you may encounter.

---

### Reporting Technical Issues
When you experience technical issues:  
- Leave a **clear description of the problem**.  
- Provide **detailed steps** on how to reproduce the error.  
- Include any **anomalies or observations** that you think are important to note.  

Submit your report on the GitHub Issues page:  
 - [GitHub Issues](https://github.com/unkyulee/micro-journal/issues)

---

### In Case of Repair Request
If your unit requires repair:  
- I can examine the device, fix it, and ship it back to you.  
- In the worst case, I can build a new one and ship it back.  

**Conditions:**  
- You must send me the unit.  
- You cover the shipping cost and any material costs (if necessary).  

Please send me a message when you are in need of a repair.

* [Reddit – Background\_Ad\_1810](https://www.reddit.com/user/Background_Ad_1810/)


---

### Disclaimer
I want to be clear so there is no misunderstanding:

- I design these writer decks for myself in my own search for the perfect writing and journaling device.  
- I design the devices, print them, purchase and assemble the electronics, and write the software.  
- I share these designs and the software, and I provide parts in a kit for those who want such a device as a **DIY (Do It Yourself) project**.  
- I also assemble these devices if people want, in which case you are also paying for my time, but they are still DIY.  

If problems occur, solutions may involve **your tinkering under the hood**. 
I enjoy sharing my designs and delight in people finding them useful. But I am **not a manufacturer**. I am handcrafting devices.  

- There is **no warranty**.  
- Although I will try to help out when people run into problems, you are not purchasing any obligation for customer support.  
- I have a job and a family, I take vacations, and I have no staff that mans the phones or responds to emails while I am away.  


