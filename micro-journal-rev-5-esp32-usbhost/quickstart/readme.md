
# **Micro Journal Rev.5 — Complete User Guide (Refined Edition)**

## **Table of Contents**

1. [Introduction](#introduction)
2. [First Steps](#first-steps)

   * [What You Need](#what-you-need)
   * [Battery Requirements](#battery-requirements)
   * [SD Card Requirements](#sd-card-requirements)
3. [Hardware Setup](#hardware-setup)

   * [Step 1 — Install the Battery](#step-1--install-the-battery)
   * [Step 2 — Prepare the SD Card](#step-2--prepare-the-sd-card)
4. [Google Drive Sync Setup](#google-drive-sync-setup)

   * [Create Folder & Apps Script](#1-create-a-folder-in-google-drive)
   * [Deploy Script & Get Sync URL](#6-deploy-the-apps-script)
   * [Configure Your SD Card](#9-edit-or-create-configjson-on-the-sd-card)
   * [Wi-Fi Setup](#10-wifi-setup)
5. [Firmware Update](#firmware-update)
6. [Using External Keyboards](#using-external-keyboards)
7. [Customizing Startup & Sleep Animations](#customizing-start-and-sleep-animation)
8. [Micro Journal Rev.5 Walkthrough](#the-micro-journal-rev-5-user-manual)

---

# **Introduction**

Welcome to the **Micro Journal Rev.5**, a portable digital typewriter designed for distraction-free writing.
This guide will walk you through the complete setup, from installing the battery to syncing your writing with Google Drive.

---

# **First Steps**

## **What You Need**

Before turning on the device for the first time, prepare:

* **One 18650 Lithium-ion 3.7V battery**
* **One SD card (1–32 GB recommended)**
* A **5V USB-A charger** (non-PD)
* **USB-A to USB-C cable**

---

## **Battery Requirements**

![Battery](./images/battery.png)

The writerDeck requires a **single 18650 Lithium-ion 3.7V battery**.

### ✔ Good to know

* Both **flat-top** and **button-top** batteries work.
* Choose **well-known brands only**.
* **Maximum real capacity is ~3300 mAh** — anything advertised above this is fake.
* Ensure the battery includes **short-circuit protection**.
* If you don’t have a battery yet, the device can run temporarily via USB power.

### 🔥 Safety Warning

**Never use AA or AAA batteries. Only 18650 Li-ion.**
Incorrect battery type may damage the device or cause fire.

### Recommended Battery

* [Flat-top, verified working (US)](https://www.amazon.com/gp/aw/d/B0CPBGDP1H)

---

## **SD Card Requirements**

![SD](./images/sd.jpg)

* Use **standard SD cards** (the larger size).
* **MicroSD + adapter works fine**.
* Prefer **low capacity (1–16 GB)**.
  Larger cards (>32 GB) require manual partitioning.
* Avoid ultra-high-speed cards — some SanDisk “Extreme” cards may not be recognized.

---

# **Hardware Setup**

## **Step 1 — Install the Battery**

![Unscrew](./images/unscrew.jpg)

### ⚠️ EXTREMELY IMPORTANT

**Double-check battery polarity (+ and –) before inserting.
Reversed polarity can permanently damage the device.**

1. Open the enclosure by removing the screws.
2. Locate the battery holder.

   * **The spring = negative (-)**
   * **Flat metal = positive (+)**
3. Insert the battery carefully.
4. Before closing the case:

   * Toggle the power switch to confirm the screen turns on.
   * Ensure **no cables are caught** when closing.
5. Charge the battery **at least 4 hours before first use**.

### Signs of low battery

* Screen flickering
* Screen turning white

Recharge if these occur.

---

## **⚠️ Important: USB Charging Limitations**

The writerDeck **does NOT support USB Power Delivery (PD)**.

Do NOT use:

* USB-C to USB-C PD chargers
* USB-C laptop chargers
* High-wattage PD power banks

Must use:

* **Standard 5V USB-A charger**
* **USB-A to USB-C cable**

Using PD chargers may prevent charging or powering the device.

---

## **Step 2 — Prepare the SD Card**

1. Insert SD card into your computer.
2. Format using:

**File System:** `FAT32`
**Allocation size:** default

![format](./images/format.png)

If FAT32 is not available:

* Create a partition **smaller than 32 GB**.

Insert the SD card into the writerDeck after formatting.

---

# **Google Drive Sync Setup**

This allows your writerDeck to back up files through Wi-Fi.

---

## **1. Create a Folder in Google Drive**

1. Visit: [https://drive.google.com](https://drive.google.com)
2. Create a folder named **uJournal**

![drive1](./images/drive_001.png)
![drive2](./images/drive_002.png)

---

## **2. Create Google Apps Script**

1. Enter the **uJournal** folder.
2. Create a new **Google Apps Script** file.

![drive3](./images/drive_003.png)

---

## **3. Add the Sync Script**

Open the latest script here:

[https://raw.githubusercontent.com/unkyulee/micro-journal/main/micro-journal-rev-4-esp32/install/google/sync.js](https://raw.githubusercontent.com/unkyulee/micro-journal/main/micro-journal-rev-4-esp32/install/google/sync.js)

Copy and paste into your Apps Script editor:

![drive4](./images/drive_004.png)

Save the project with any name.

---

## **6. Deploy the Apps Script**

1. Click **Deploy → New deployment**
2. Select **Web app**

![drive5](./images/drive_005.png)
![drive6](./images/drive_006.png)

3. Configure as follows:

* **Execute as:** Me
* **Who has access:** Anyone

![drive7](./images/drive_007.png)

4. Press **Deploy**, authorize the script, and accept the warning.

![drive8](./images/drive_008.png)
![drive9](./images/drive_009.png)
![drive10](./images/drive_010.png)

---

## **8. Copy Your Web App URL**

![drive11](./images/drive_011.png)

You will need this URL for your `config.json`.

---

## **9. Edit or Create `config.json` on the SD Card**

On your SD card, open or create a file named:

```
config.json
```

Use this template:

```json
{
  "sync": {
    "url": "PASTE YOUR WEB APP URL HERE"
  }
}
```

Be careful not to remove commas, quotes, or braces.

Insert the SD card back into the writerDeck.

To test sync:

* Press **ESC**, then **S**
* Ensure Wi-Fi is already configured.

### ⚠ Wi-Fi Notes

* Only **2.4 GHz** networks are supported.
* 5 GHz Wi-Fi cannot connect.

---

## **10. WiFi Setup**

1. Press **ESC**
2. Press **W**
3. Select a profile number
4. Enter SSID
5. Enter password
6. Connection will be tested automatically

You may save up to **5 Wi-Fi networks**.

---

# **Firmware Update**

Keeping firmware updated ensures stability and new features.

## **1. Check Current Version**

Press **ESC** or **M** to open the menu.

If you see:

> **Version 1.0.0**

You *must* update.

![firmware](./images/firmware_001.jpg)

---

## **2. Download Latest Firmware**

Get the newest version from:

[https://github.com/unkyulee/micro-journal/releases](https://github.com/unkyulee/micro-journal/releases)

Download:

```
firmware_rev_5.bin
```

---

## **3. Install the Firmware**

1. Power off the device
2. Remove SD card
3. Copy the firmware file to the SD card
4. Insert SD card back into the writerDeck
5. Turn it on

The screen will turn white for ~10 seconds, then reboot with the new firmware.

If update does not trigger:

* Rename file to `firmware.bin` and try again.

---

# **Using External Keyboards**

The Rev.5 relies heavily on your external keyboard.

### ✔ Supported

* Wired USB keyboards
* Wireless 2.4 GHz dongle keyboards
* Bluetooth keyboards (via BLE menu)

### ✖ Not recommended / incompatible

* Keyboards with USB hubs
* Keyboards that charge through USB (power draw too high)
* RGB/gaming keyboards with heavy LED load
* Anything drawing high current

**High-power keyboards may damage the Micro Journal.**

---

# **Customizing Start and Sleep Animation**

You can replace the two animated GIFs:

* **wakeup.gif** → plays at boot
* **sleep.gif** → plays after 1 minute of inactivity

### GIF Requirements

* **320 × 240 px**
* **≤ 1 MB each**

Place them directly on the SD card.

Create or convert GIFs here:
[https://ezgif.com/](https://ezgif.com/)

---

# **The Micro Journal Rev.5 User Manual**

v1.2

Prepared by Hook

```
I am not Un Kyu Lee, so any mistakes in this document are likely mine and not his. 

- Hook
```



## Introducing the Micro Journal Rev 5

The Rev 5 is one of a line of Micro Journal Writer Decks aimed at (my words) *writing focused* drafting. That is, they are digital typewriters that help you focus on getting the words out when you are drafting. The Rev 5 is the simplest of this line. It is, for the most part, a screen with editing software. You need to add a keyboard to use it. Firmware-wise, it is very similar to a Rev 6, except you may not have to carry a keyboard with you if you are going someplace with a spare keyboard. However, you may want to find a smaller, lightweight keyboard you can use with it. But, ultimately, you don't need to buy a keyboard if you already own keyboards.

Not all keyboards will work, but most will. Wired is easiest, just attach it to the USB port on the back of the Rev 5. But Wireless dongle keyboards should work and BT keyboards should work. However, there are a few things to avoid. In general, don't have a hub between the Rev 5 and a wired keyboard. For wired keyboards, avoid any that have their own internal battery, or draw  excessive power (as might be indicated by lights and LEDs. This isn't an issue for BT keyboards since they are not being connected directly. If wireless keyboards don't work it is likely to be a protocol or signal problem on the part of the keyboards.

We will not be concerned here with the keyboards. There are so many. This guide will assume the user has found a keyboard that works and just run through the features offered by the software.


# Using the Micro Journal Rev 5 - A Walkthrough

## Setting Up the Micro Journal Rev 5

For setting up your Rev 5, I really can't do much better than Un Kyu lee has already done with his quick start guide, here:

https://github.com/unkyulee/micro-journal/blob/main/micro-journal-rev-5-esp32-usbhost/quickstart/readme.md

Before we dive in, **make sure you have the latest stable firmware for the Rev 5** before you proceed any further. If a version is marked [DEV], that is a version in development and could have bugs, although it may have desirable features and fixes. Read the changelog and decide which version you want. You can download the current firmware from here:

https://github.com/unkyulee/micro-journal/releases

Then, just remove the SD card from the Rev 5 and use a card reader with your computer to transfer the firmware file to your SD card. Put the SD card back in the Rev 5. When you turn on the Rev 5, you will be asked to acknowledge loading the new firmware.  Then, after some screen flashing, you will be set to go.

## Using the Rev 5 Part 1: The Editor

For right now, we are only considering the Rev 5's editor and it's very simple feature set. We will save other features like WIFI and syncing for the next section when we look at the menu. The Editor is the core functionality of the Rev 5. With an added keyboard, it becomes a digital typewriter to help you get your words out with fewer distractions.

The editor is very basic, but drafting and journaling don't require a very large feature set. It types characters. It has a 320 x 240 pixel color LCD screen, which, with the font size used gives you 11 rows with about 25 characters across. The text is quite sharp, clear and readable assuming good vision (including good vision correction). There is only one thing this editor does. It types words in plain text format, saves them as you go and allows you to navigate through the text with arrow keys if you want to check something, delete or backspace. There is no copy and paste, no spell check, no paragraph formatting (although you can do that with markdown if you wish) and no AI. Best of all, there is no need to be connected to the internet to use this editor and save your texts. If you are distracted, it won't be because of anything on the Rev 5.

The editor has 10 filespaces, labeled 0-9. These filespaces are where you do your writing. This is very much like the Neo 2 except you don't have dedicated keys to open the different file spaces. Instead you either use the menu to select filespaces.

When you first turn on the Rev 5, you are instantly placed in the first filespace, numbered 0. The screen is blank except for a status area at the very bottom of the screen. In the status area you will see (left to right) the filespace number, the word count and, to the far right a green dot. When you start typing, the red dot will change to red, meaning there is unsaved typing, and the word count will start updating. When writing, words appear at the bottom and are pushed up, a typewriter metaphor. The text word-wraps with Enter creating a new paragraph. 

The Rev 5 doesn't save letter-by-letter, but will save when you pause your typing. You will see the red dot turn to green whenever you pause. You are safest if you wait (it isn't a long one) until the dot turns green before turning off the Rev 5. And that's it. You type. You can use arrow keys to navigate to review or make small local fixes. And you can check your word count. By the way, subsequent to this first session, the Rev 5 will start up in whatever file space you were working in last. 

Be aware that the buffer that maintains the text on your screen, allowing you to navigate back to review the text, is limited. When the buffer limit is exceeded, text at the top of the buffer (which will be the earliest text you typed) is lost in the sense that you can't navigate to review it. However, the text is still in your file on the SD card. Nothing should ever be lost. However, this is also how this editor is different from more elaborate editors on your computer. It cannot load files from off the SD card, it can only save text you are creating.

So, you have turned on the Rev 5 and written some text. Now, what can you do with that text? Do you want to edit it on your computer? How do you get it there? Can you sync directly or to a cloud? What settings are available to change your experience of the editor? For these questions or more, we turn to the menu system. 


## Using the Rev 5 Part 2: The Menu Screen

You can exit the Editor to the menu screen one of two ways. You can either use the Esc key on your keyboard or you can press the M button on the front of the Rev 5. There are two buttons under the screen on the Rev 5 that can be used any time, even without a keyboard attached. The M button, which takes you to the Menu screen and a button marked B which issues the "Back" command, usually used on the menu screen or to exit a menu function.

The Menu has several types of things. The top of the screen shows what firmware version you have. On the right hand side of the menu screen is a list of the 10 file spaces.  You can change filespace here by simply typing a number. You can also delete the contents of a file space here by typing "D" on this screen. It will delete the contents of the last file space you were working in (you'll see the number of the filespace to be deleted in parentheses next to the Clear file item.

On the left side of the menu are are various settings and functions that affect the editor or allow you to backup you text files to other devices or the cloud. Each one is engaged by typing the single letter in brackets next to them. They are:

* ***[S] SYNC***
* ***[K] KEYBOARD LAYOUT***
* ***[F] DEVICE BUTTON***
* ***[M, MENU] BLE KEYBOARD***
* ***[W] WIFI***
* ***[G] BACKGROUND COLOR***
* ***[C] FONT COLOR***

* ***[B] BACK***

Lets go through each of them one by one.

**SYNC** - This requires WIFI (the next menu item) to be set up first and it requires the Google Apps Script you set up in the Quickstart guide (which  is done on your computer, and then the app link is added to the config.json file on your Rev 5). If everything is set up correctly, Typing "S" on this menu will send whatever was in the file you were just working in to Google Drive. If you did this for another Micro Journal already you can reuse the same link without going through the whole setup again.

**KEYBOARD LAYOUT** - This allows you to select which standard international keyboard layout you want to use, such as US, UK, French (AZERY), etc.  There is one standard layout option on this list that is not tied to language and that is DVORAK.  Select the letter that represents your choice and you will be returned to the last filespace you were working in. These are standard layouts in each case.

**DEVICE BUTTON** - This just determines whether you use the device buttons under the screen. The default is that they function. You cannot assign different functions to them.

**BLE KEYBOARD** - "BLE" stands for Bluetooth Low Energy. It allows a limited range connection from a Bluetooth Keyboard. The first time with any device you will have to do the pairing, but after that the connection should be automatic once you invoke it on the Rev 5 by typing either "M" or pressing the Menu button on the front of the Rev 5.

**WIFI** - Typing "W" on the menu screen will take you to the Wifi settings screen. You only should need to do this once, though you can have up to 5 WIFI profiles if you take the Rev 6 to different locations. Type the number of the profile you want to set up or edit. You will be asked for an SSD and a password for that SSD. The software will then test the connection. If it succeeds, you are done.  Wifi doesn't stay on, there's no reason for it to do so.  It is only used for syncing to Google Drive. Running Sync will turn on WIFI, sync and turn off Wifi.

<u>*Note*</u> The Rev 6 can only connect to 2.4 Ghz Wifi. It won't connect to 5Ghz WIFI. Sometimes, especially if you use a router that offers both in a mode where the router decides which is appropriate for the device trying to sync, you may have to go through the setup more than once until the router gets it right. After that, you won't have any more trouble.

**BACKGROUND COLOR** - This lets you customize the background color of the editor.

**FONT COLOR** - This lets you customize the color of the font in the editor.

**BACK** - Typing B on the Menu returns you to the editor in the last filespace you were working in.


You'll notice there is no Keymap customization option beyond the standard layouts provided on the menu. That shouldn't be a mystery. The Rev 5 has no keyboard and simply relies on whatever keyboard is attached (USB, BT or Wireless) to provide the character map. The Rev 5 is a very simple Writer Deck.

You should be ready to jump in and write. If you have any questions, feel free to drop by and post them here: https://www.flickr.com/groups/alphasmart/

Happy writing!

