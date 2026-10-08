# Micro Journal Rev.8: Melodica - a Distraction-free Writing Machine

A portable keyboard with a built-in screen, made only for writing. No apps, no notifications, no browser tabs. Switch it on and start typing within a second.

### What you get

* **Instant:** boots in about 1 second, so a thought never has to wait.
* **Readable anywhere:** the screen gets clearer in sunlight, like paper, and has no backlight glare.
* **A real keyboard:** mechanical switches you can swap for your own favorites.
* **Your words, your way:** copy files over USB, send them by Bluetooth, or back them up to your own Google Drive. No subscription.
* **Runs on a standard 18650 battery** that you can replace yourself.


---

## Choose your version

|                               | **DIY Kit**                                                                                                  | **Assembled, without switches and keycaps**               | **Fully assembled**                                             |
| ----------------------------- | ------------------------------------------------------------------------------------------------------------ | --------------------------------------------------------- | --------------------------------------------------------------- |
| **For whom**                  | You enjoy building it yourself. You should be comfortable soldering and have a bit of electronics knowledge. | You want a built device and your own switches and keycaps | You want to start writing right away                            |
| **What arrives**              | All the parts, ready to assemble (see list below)                                                            | The device fully assembled, with an empty keyboard        | The device fully assembled, with switches and keycaps installed |
| **You still need to provide** | Keyboard switches, keycaps, and one 18650 battery                                                            | Keyboard switches, keycaps, and one 18650 battery         | One 18650 battery                                               |

> **Important: the DIY Kit does not include keyboard switches or keycaps.**
> Only the *Fully assembled* version comes with them. Tindie cannot hide product options for a specific version, so you will see switch and keycap choices on every version. If you order the DIY Kit or the version without switches, select **N/A** for those options. 

#### What is in the DIY Kit

* 3D printed parts
* Pre-soldered keyboard PCB
* Microcontroller: ESP32-S3 with Micro Journal firmware loaded
* Reflective LCD display and breakout board
* LiPo battery charger module
* 18650 battery holder
* Power switch
* Various M3 and M2 hex screws, and heat inserts

#### Please note

* **Only the Fully assembled version includes switches and keycaps.** The DIY Kit and the version without switches come with an empty keyboard, whatever the options on the listing show.
* **The battery is NOT included in any version.** Airplane delivery restrictions do not allow batteries in the parcel. The device needs one 18650 battery to operate, so please source it separately.
* For the version without switches, use any Cherry MX compatible switches (3-pin or 5-pin). XDA or MOA keycaps are recommended.


---

## Make it yours

### Switch types

**Clicky**
* A noticeable click and tactile bump.
* Clear feedback when the key activates.
* Preferred by writers and typists who enjoy feeling each keystroke.

**Silent**
* Very quiet, ideal for shared or quiet environments.
* Smooth, gliding key feel.
* Less finger fatigue during long typing sessions thanks to the smooth travel.

Here is a video comparing the two types:

* [Silent Switch VS Clicky Switch Comparison (VIDEO)](https://youtu.be/9zu-tkawMV0)

### Colors

You can request custom colors. Please contact me as soon as you have placed your order, and make sure to **get confirmation** of your color options.

Here are the color options already chosen by previous customers:

* [Micro Journal Rev.8 Colors](https://www.youtube.com/playlist?list=PLrUXYLEnAaNSl_UlYStnZSTiyyWXFFlb8)

At the beginning the playlist will not show many colors, probably because not many have been built yet. To see more options, have a look at my YouTube channel for the colors of other Micro Journal models and see if any of them fit you.

* [YouTube - @unkyulee](https://www.youtube.com/@unkyulee)


---

## What it is, and what it isn't

Micro Journal Rev.8 is a **drafting tool**. It is made for getting your words down without distraction. It is **not a word processor**.

**Good for**
* Writing first drafts, journals and notes, indoors or outdoors.
* Simple editing: you can move around in your text and fix it as you write.
* Keeping your writing off your phone and out of your browser.

**Not made for**
* Formatting, fonts, images or layout.
* Heavy editing of long documents. Plan to finish and polish your text on your computer.

**Good to know**
* Ten file slots.
* The screen is monochrome (black dots on a light background).
* Supports UTF-8, so many languages are possible. Currently most EU languages and Korean are supported. If you would like to see your language added, please message me and we will find a way to add it to the device.

Please watch the demo video to understand the features and, most importantly, the limitations of the device:

* [Micro Journal Rev.8: Melodica - Features and Use Cases](https://youtu.be/VGHCE0Egx4Q)


---

## The story behind it

This time, I tried to build a writing machine that uses a reflective LCD.

The interesting thing about this display is that it becomes clearer under brighter light. Unlike most displays, it does not fight against ambient light. Instead, it benefits from it, a bit like what e-ink screens are famous for.

The screen is also monochrome, showing only black dots on the display. That simplicity makes it pleasant to look at. It has some of the character of an e-ink display, but with a much faster refresh rate.

Since there is no light source behind the screen, it feels very easy on the eyes and remains enjoyable even during long writing sessions.

For a writing machine, I feel like this could be it. Maybe this is the end of my personal journey. The final chapter, where everything finally finds its shape and the story reaches a satisfying closure.

I really think this build is amazing for writing. Simple, readable, and fast enough to keep up with your thoughts.

* [More stories ...](https://github.com/unkyulee/micro-journal/blob/main/rev.8/8.0/story.md)



---

## ⚠️ Please read before you buy

### I am not a mass-manufacturer.

I design these writer decks for myself, in my own search for the perfect writing and journaling device. I design them, print them, buy and assemble the electronics, and write the software, all by hand. I share the designs and software, and offer parts as a kit for anyone who wants to build one.

**Every version is a DIY project, including the fully assembled one.**
If you choose *Fully assembled*, you are paying for my time to build it. You are not buying a finished consumer product. These are handcrafted devices, and if a problem comes up, the solution may involve some tinkering under the hood. **Please buy only if you are willing and able to do that tinkering yourself.** I am not responsible if you cannot get a device working or fix it, even if you bought it fully assembled.

**No warranty.**
I will always try to help when something goes wrong, but only within reasonable limits. Buying a device does not buy you an obligation of customer support. I have a job and a family, I take vacations, and I have no staff to answer emails while I am away.

If that does not suit you, a mass-manufactured device may be the better choice. If it does, I am glad to share my work with you.


---

## For makers: Enjoy the Open Source

Everything is open. If you want to see how it works, build it yourself or change it, start here.

### Specs at a glance

|                      |                                                                      |
| -------------------- | -------------------------------------------------------------------- |
| **Microcontroller**  | ESP32-S3                                                             |
| **Display**          | Monochrome reflective LCD (RLCD), readable in sunlight, no backlight |
| **Keyboard**         | 65%, hot-swappable, Cherry MX compatible (3-pin or 5-pin)            |
| **Battery**          | One 18650, user replaceable                                          |
| **Boot time**        | About 1 second                                                       |
| **Getting text out** | Web Editor, Bluetooth (BLE), Google Drive Sync                       |
| **Storage**          | Ten file slots                                                       |

### How your text gets out

1. **Connect to your PC.** Plug in a USB cable and the device shows up as a drive with all your files.
2. **Send via Bluetooth (BLE).** Rev.8 can act as a Bluetooth Low Energy keyboard. Press "SEND" and it types out all the text you have written.
3. **Sync to Google Drive.** Set up your own Google Drive and back up your files over Wi-Fi. There is no third-party cloud service and no subscription. The device connects to Wi-Fi only during the sync.

### Documentation and files

* [Quick start guide](https://github.com/unkyulee/micro-journal/blob/main/rev.8/8.0/guide.md)
* [Build guide](https://github.com/unkyulee/micro-journal/blob/main/rev.8/8.0/build.md)
* [Design files (STL)](https://github.com/unkyulee/micro-journal/tree/main/rev.8/8.0/STL)
* [Micro Journal Rev.8 documentation](https://github.com/unkyulee/micro-journal/tree/main/rev.8/8.0)
* [Using Micro Journal Rev.6 as a Bluetooth keyboard](https://youtu.be/IW5ninGiN7k)


---

## Practical info

### Ordering from Korea? / 한국에서 주문하시는 경우

한국에서 주문을 고려하시는 경우 아래 사이트에 이메일 등록해주시면, 제작 가능할 때, 바로 연락을 드리겠습니다.

If you are considering an order from Korea, please register your email at the site below. I will contact you as soon as a unit is available to build.

[https://www.yesbut.it/sales](https://www.yesbut.it/sales)

### In case of repair request

If your unit needs a repair, I can have a look, fix it and ship it back. In the worst case, I can build a new one and ship it to you, provided that you send me the unit and cover the shipping and material costs. Send me a message when you need a repair.

### Community

* [Un Kyu Lee's Design Gallery](https://www.yesbut.it/)
* [YouTube – @unkyulee](https://www.youtube.com/@unkyulee)
* [Reddit – Un Kyu Lee](https://www.reddit.com/r/unkyulee/)
* [Micro Journal Rev.8 Discussion Forum](https://www.flickr.com/groups/alphasmart/discuss/72157721925271377/)