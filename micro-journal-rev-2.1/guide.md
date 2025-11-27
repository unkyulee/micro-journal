# Quick Start Guide - Micro Journal Rev.2.1: cyberDeck


## **Things to Buy**

#### **Charger Adapter**

Use a **USB-A wall charger** that can supply **at least 2A**.
The device **does not support USB Power Delivery (PD)**. This means:

* **PD chargers will not work**
* **USB-C to USB-C cables will not charge nor power the device**

You must use a **USB-A charger** with a **USB-A to USB-C cable**.

*Example:*
[https://www.amazon.com/MEHIDFY-USB-Wall-Charger-Charging/dp/B0BZCY3HF9/](https://www.amazon.com/MEHIDFY-USB-Wall-Charger-Charging/dp/B0BZCY3HF9/)

---

#### **USB-A to USB-C Cable (for Charging & Power)**

You will need a **USB-A to USB-C cable** capable of delivering **up to 15W (5V/3A)**.
Check that the cable specifications explicitly support **3A or higher** current output.

*Example:*
[https://www.amazon.com/Amazon-Basics-Charger-480Mbps-Certified/dp/B01GGKYN0A/](https://www.amazon.com/Amazon-Basics-Charger-480Mbps-Certified/dp/B01GGKYN0A/)

---

#### **Battery (Optional)**

The device supports **up to four 18650 lithium-ion batteries**, but can also run **without any batteries installed**.
For stable performance, it is recommended to install **at least two** batteries.
Make sure the batteries are **fully charged** before first use.

Helpful links:

* **Battery Buying Guide:**
  [https://www.reddit.com/r/18650masterrace/comments/qp21o8/buying_18650_batteries_start_here/?share_id=RTqe02Hqv2l_VwJTwoIi1&utm_content=2&utm_medium=android_app&utm_name=androidcss&utm_source=share&utm_term=1](https://www.reddit.com/r/18650masterrace/comments/qp21o8/buying_18650_batteries_start_here/?share_id=RTqe02Hqv2l_VwJTwoIi1&utm_content=2&utm_medium=android_app&utm_name=androidcss&utm_source=share&utm_term=1)

* **Verified Working Battery (Flat-top, US):**
  [https://www.amazon.com/gp/aw/d/B0CPBGDP1H](https://www.amazon.com/gp/aw/d/B0CPBGDP1H)

Search for **“18650 Li-ion flat-top”** or **“LiPo 18650”** to find compatible batteries. These are the most commonly used by current Micro Journal users.


---

## **How to Power the Device**

#### ⚠️ Important: Use Only Non-PD USB Chargers

The Micro Journal **does not support USB Power Delivery (PD)**.
This means:

* **PD chargers will not work**
* **USB-C to USB-C cables will not power or charge the device**
* The device may appear dead or unresponsive if powered with the wrong charger

To power the device correctly, **you must use a standard 5V USB-A charger** (such as an older phone charger) **together with a USB-A to USB-C cable**.
Using the proper charger ensures reliable power and protects the device from damage.

---

### **Using the Device Without Batteries**

You can run the Micro Journal Rev.2.1 entirely from USB power.

1. **Connect a USB-A to USB-C cable** to the USB-C charging port located at the back of the device.
2. **Turn the power switch ON.**
3. You should see:

   * A **red light** glowing at the back (power indicator)
   * A **green blinking light** under the front grill (Raspberry Pi activity / boot indicator)
4. **To turn the device off:**

   * Perform a **soft shutdown** from the menu
   * Wait until the screen is fully off
   * Ensure the **green light stops blinking**
   * Then turn the switch to the **OFF** position

---

### **Installing Batteries**

The device can operate using up to **four 18650 lithium-ion batteries**.

1. Remove the **two small screws under the screen** to access the battery compartment.
2. Installing the **fourth battery** requires opening the left side of the enclosure (remove 4 Torx screws).
3. Video instructions are available here:
   **[Battery Installation Guide](https://youtu.be/-P8MK0iT04k?si=TgdXtUZAtJaRblRU&t=400)**

---

### ⚠️ Important Battery Safety & Setup Notes

#### **⚠️ Do NOT connect the charging cable while inserting batteries.**

Insert all batteries first, with the device fully disconnected from power.

#### **⚠️ You must charge the batteries at least once after installation.**

The battery module **won’t activate** until it detects a charge cycle.
After inserting batteries:

1. Connect your USB-A charger
2. Allow the device to begin charging
3. After this first charge, battery power will work normally

#### **⚠️ Pay close attention to battery polarity.**

Incorrect polarity can cause **short circuits, overheating, or fire**.
Match the + and – signs carefully before inserting each battery.

---

#### **Power Requirements & Important Warnings**

This device does **not** support USB Power Delivery (PD). Do **not** use PD chargers or USB-C to USB-C cables, as the device will not power on with them. You must use a **standard USB-A charger** (such as an older phone charger) together with a **USB-A to USB-C cable**.

If the screen **flickers during boot**, this indicates that the power supply is insufficient. Ensure that your charger and cable can reliably provide **at least 2A**. Lower-quality cables or low-power adapters may cause unstable operation. Reducing the display brightness can also help lower the power draw—press the button behind the display to cycle through the available brightness levels.


---

### **Powering On After Battery Installation**

* Turn the switch to the **ON** position.
  The device should power on normally.
* If it does not turn on, **press the power reset button once**.
* Pressing the reset button **twice** or **holding it** will turn the battery module **off**.
  You will notice the **red light turning off** when the battery module is disabled.



### When it doesn't Boot

**Initial delay:**  Rev 2.1 is not instant on. It has to boot Linux first, since everything runs off that. If you haven't used Linux before, you will see a lot of incomprehensible text go whizzing by too fast to read. At first the text will fly by in the wrong direction on the screen and finally it will reorient. Just ignore everything until you are given the terminal dashboard.

**Check the SD card:** If the screen remains blank for too long, check for a green light on raspberry pi. This light indicates that the system is booting. If no green light appears, the SD card may not be properly installed. 

<img src="./quickstart/008.png" width="400" />

**When not booting** Power off, reinsert the SD card, and try again. If the issue persists, you may need to replace or reconfigure the SD card. 99% of the issues is related to SD card. Please, refer to this document when you need to setup the SD card.


---

## Setup Micro SD Card

**Note: A pre-configured Micro SD card is included with the Micro Journal Rev.2.1. You do not need to buy or set it up. Refer to this guide only if you need to replace or reinstall the Micro SD card.**

The Micro Journal Rev.2 uses a Micro SD card for its Raspberry Pi Zero 2W computer. The card should have a capacity of **at least 4GB**. Using a faster card will improve boot times and overall performance.

While your Micro Journal Rev.2 ships with the Micro SD card already installed and configured, you may need to set up a new card in the future. For detailed instructions on how to do this, refer to the following guide:

[Raspberry Pi SD Card Setup How-To](https://github.com/unkyulee/micro-journal-linux/)

Default configuration is designed to be user-friendly, allowing you to quickly get into writing mode without needing to learn complex Linux terminal commands.


### Note for Advanced Users

If you are familiar with Raspberry Pi and Linux configurations, feel free to customize the setup. The Micro Journal Rev.2 has no major constraints, though a minor display configuration may be necessary. Beyond that, you have the freedom to configure the Raspberry Pi as you prefer.

---


## Using the Dashboard

After the device has fully booted up, you will be greeted with the **Dashboard**. 

The dashboard is a kind of menu. It really is a crude file manager, but you have one folder (documents) and several scripts (files ending with .sh-- if you ever used DOS, think batch files) and you use the arrow keys to select among them, so it acts like a menu.


The documents folder is obviously where your files will be stored. You use the right arrow to move to the list of file names in that folder and hit enter to open any file in WordGrinder. To create a new file, you arrow down from the documents folder to the script called newfile. We will get to WordGrinder in a moment.

Use the **cursor keys** to navigate the options on the screen. You can press the **up** and **down** arrows to move between selections.
  
To create a new document, select **"newfile.sh"** and press **Enter** or the **right cursor key**. This will open **WordGrinder**, where you can begin typing.

To save your work in WordGrinder, press **Ctrl + S**, and to quit, press **Ctrl + Q**.

Here are some more commands that you can utilize in the dashboard.

https://gist.github.com/heroheman/aba73e47443340c35526755ef79647eb


---

## WiFi Setup

To connect to a WiFi network, follow these steps:

1. Navigate to **"config.sh"** on the dashboard and press **Enter** to open the Raspberry Pi configuration screen.
2. Select **"1 System Options"** and press **Enter**.
3. Select **"S1 Wireless LAN"** and press **Enter**.
4. Enter the **SSID** (name) of your WiFi network.
5. Enter the **password** for your WiFi network.

Make sure that the wifi system is in 2.4 Ghz. 
Also, try to seting the WiFi country. 5 Localisation Options -> L4 WLAN Country

---


## How to Transfer Your Files

To share or access files from the Micro Journal, follow these steps:

1. Select **"share.sh"** from the dashboard and press **Enter**.
2. Wait a few seconds, and once you see the **IP address**, enter it in the web browser of your PC.

You will be greeted by a web interface where you can download, manage, and extract files stored in the **"documents"** folder on your Micro Journal.


#### Using Dropbox to sync files

Detailed guide on how to setup Dropbox via Rclone written by HollerPots:

https://hoolis.net/posts/rev2.html

---


## Getting into Writing

You are now ready to go. Select **newfile.sh** on the dashboard and hit enter and you are in [WordGrinder](https://cowlark.com/wordgrinder/index.html). It will tell you it can't load your file (of course not... it's a new file so it doesn't exist yet). Hit space and you will be in the new, blank WordGrinder file. Quite honestly you can just type, and use **ctrl-s** to save and **ctrl-q** to quit. But WordGrinder has a complete menu system you access with the **Esc** key. Wordgrinder saves in a non-standard Text format. Luckily, WordGrinder has it's own export function to convert it to text or pdf.

All your files go into the documents folder, including the exported text and pdf files. You then get access to them on your computer by exiting WordGrinder and selecting the **share.sh** script. It will give you an ip address. Type that ip address into your browser on your computer (or, I assume, phone) and you will be able to see those files, open them and copy and paste.

You may need to be in the same wifi network as the Micro Journal in order to access the share features.


#### Note on WordGrinder Files:

WordGrinder saves files in its own format. To read or share them easily, you must export the file as either **PDF** or **Plain Text**. To export a document, press the **ESC** key while in WordGrinder to access the export options.


#### Adding Spellcheck Feature 

https://www.reddit.com/r/writerDeck/comments/1ja4hdg/adding_spellcheck_feature_to_micro_journal_rev2/

Credit to: Misplaced_Fan_15

#### Using NeoVim as an editor

https://www.reddit.com/r/writerDeck/comments/1jf4fqm/micro_journal_rev_2_with_neovim_as_word_processor/



