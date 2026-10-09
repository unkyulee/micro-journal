# Micro Journal Rev.5.1: A Personal Journey - a Writing Deck for Your Own Keyboard

Craft your own personalized writerDeck by pairing it with your favorite mechanical keyboard. This unique device seamlessly connects to a vast array of mechanical keyboards via a USB port, transforming them into functional writerDecks. With the ability to power on and start typing immediately, along with the added convenience of syncing with Google Drive, this writerDeck ensures that your drafts are always securely recorded.

Powered by the ESP32-S3 microcontroller, this device offers instant writing capabilities. It provides a user-friendly tool for all your writing needs, reminiscent of popular commercial alternatives.

Upon powering up, this device springs to life within moments, inviting you to unleash your creativity. With seamless integration with Google Drive, your work is effortlessly stored in your preferred cloud system, offering peace of mind and accessibility from anywhere. Whether capturing fleeting ideas or jotting down important tasks, this writerDeck is always ready to assist. Keep it on your bedside table to capture your dreams, and simply flip the power switch whenever inspiration strikes.

### What you get

* **Bring your own keyboard:** connect your favorite USB keyboard and start writing.
* **Instant:** powers on immediately, so a thought never has to wait.
* **Distraction-free:** a screen and an editor, made only for writing.
* **Your words, your way:** back up your files to your own Google Drive. No cloud service fees.
* **Foldable design** that protects the screen when you carry it.
* **Runs on a standard 18650 battery** that you can replace yourself.


---

## Choose your version

|                               | **DIY Kit**                                                                                                  | **Assembled**                                    |
| ----------------------------- | ------------------------------------------------------------------------------------------------------------ | ------------------------------------------------ |
| **For whom**                  | You enjoy building it yourself. You should be comfortable soldering and have a bit of electronics knowledge. | You want to start writing right away             |
| **What arrives**              | All the parts, ready to assemble (see list below)                                                            | The DIY Kit, assembled before shipping           |
| **You still need to provide** | A USB keyboard and one 18650 battery                                                                         | A USB keyboard and one 18650 battery             |

> **Important: this device has no keyboard of its own.**
> The Micro Journal Rev.5.1 is a screen with a built-in editor. You connect your own keyboard to it. A keyboard is not included in any version.

#### What is in the DIY Kit

* 3D printed parts
* Microcontroller: ESP32-S3 with Micro Journal firmware loaded
* Display: ST7789
* LiPo battery charger module
* 18650 battery holder
* Power switch, 2 x 10mm M2 screw and nut
* USB A female
* Various M3 and M2 hex screws, and heat inserts

#### Please note

* **The battery is NOT included in any version.** EU delivery restrictions do not allow batteries in the parcel. The device needs one 18650 battery to operate, so please source it separately.
* **An SD card is not included, and you do not need one.** From firmware version 2.x, your texts are saved in the internal memory of the device.
* **Use a standard 5V USB-A charger with a USB-A to USB-C cable.** The device does not support USB Power Delivery (PD), so PD chargers may not charge it.

#### Which keyboards work

* Wired USB keyboards
* Wireless 2.4 GHz dongle keyboards
* Bluetooth keyboards

Keyboards that draw a lot of power are not recommended: keyboards with built-in USB hubs, keyboards that charge through USB, and RGB or gaming keyboards with a heavy LED load.


---

## Make it yours

### Colors

You can request custom colors. Please contact me as soon as you have placed your order, and make sure to **get confirmation** of your color options.

Here are the color options already chosen by previous customers:

* [Micro Journal Rev.5.1 Colors](https://www.youtube.com/playlist?list=PLrUXYLEnAaNT9xCD-dFa0QLdjVJLV7N7T)

### Keyboard layouts

Multi-language keyboard layouts are supported:

* US Layout
* US International
* Canadian Multilanguage Layout
* Italian Layout
* German Layout
* Belgian Layout
* UK Layout
* Finnish
* Swedish
* Latin America

Please request it in your order when you need a layout that is not in the list.


---

## What it is, and what it isn't

Micro Journal Rev.5.1 is perfect for capturing sudden bursts of inspiration, or for quickly jotting down important tasks and ideas. It is a device that can be kept on your bedside table, ready to record your dreams at a moment's notice. All you need to do is find the power switch before immortalizing your weirdest dream ever in writing.

**Good for**
* Writing first drafts, journals and notes.
* Simple editing: you can move around in your text and fix it as you write.
* Keeping your writing off your phone and out of your browser.

**Not made for**
* Formatting, fonts, images or layout.
* Heavy editing of long documents. Plan to finish and polish your text on your computer.

**Good to know**
* Ten file slots.
* Dimension 14 x 12 x 5 cm, 600 g.

Please watch the video to understand the features and use cases of the device:

* [Micro Journal Rev.5.1 Features and Use Cases](https://youtu.be/4LoZSAb_AXQ)
* [The story behind it](https://github.com/unkyulee/micro-journal/blob/main/rev.5/5.1/story.md)


---

## ⚠️ Please read before you buy

### I am not a mass-manufacturer.

I want to be clear so there is no misunderstanding. I design these writer decks for myself, in my own search for the perfect writing and journaling device. I design the devices, print them, purchase and assemble the electronics, and write the software. I share these designs and the software, and provide parts in a kit for those who want such a device as a DIY (Do It Yourself) project.

**Every version is a DIY project, including the assembled one.**
I also assemble these devices if people want, in which case you are also paying for my time, but they are still DIY. I am not a manufacturer, I am hand crafting devices. If problems occur, solutions may involve your tinkering under the hood.

**No warranty.**
I will always try to help when people run into problems, but within reasonable limits. You are not purchasing any obligation for customer support. I have a job and a family, I take vacations, and I have no staff that mans the phones or responds to emails while I am away.

If you are not comfortable with my limits, then you may want to consider some other mass-manufactured device. If you are, I enjoy sharing my designs and delight in people finding them useful.


---

## For makers: Enjoy the Open Source

Everything is open. If you want to see how it works, build it yourself or change it, start here.

### Specs at a glance

|                      |                                                                  |
| -------------------- | ---------------------------------------------------------------- |
| **Microcontroller**  | ESP32-S3                                                         |
| **Display**          | ST7789 LCD                                                       |
| **Keyboard**         | Not included. Connect your own through the USB port.             |
| **Battery**          | One 18650, user replaceable                                      |
| **Boot time**        | Immediate                                                        |
| **Getting text out** | Drive Mode (web browser over Wi-Fi), Google Drive Sync           |
| **Storage**          | Ten file slots                                                   |
| **Size and weight**  | 14 x 12 x 5 cm, 600 g                                            |

### How your text gets out

1. **Drive Mode.** Open, download and upload the files on the device from a web browser, over Wi-Fi.
2. **Sync to Google Drive.** Set up your own Google Drive and back up your files over Wi-Fi. There is no third-party cloud service and no subscription. The device connects to Wi-Fi only during the sync.

### Documentation and files

* [Quick start guide](https://github.com/unkyulee/micro-journal/blob/main/rev.5/5.1/guide.md)
* [Rev.5 user manual](https://github.com/unkyulee/micro-journal/blob/main/rev.5/5.0/guide.md) (the Rev.5.1 has the same internal hardware and software)
* [Build guide](https://github.com/unkyulee/micro-journal/blob/main/rev.5/5.1/build.md)
* [Google Drive Sync setup guide](https://github.com/unkyulee/micro-journal/blob/main/shared/GoogleDriveSync/readme.md)
* [3D design files (STL)](https://github.com/unkyulee/micro-journal/tree/main/rev.5/5.1/STL)
* [Firmware release page](https://github.com/unkyulee/micro-journal/releases)
* [Firmware source code](https://github.com/unkyulee/micro-journal-mcu)
* [Micro Journal Rev.5.1 documentation](https://github.com/unkyulee/micro-journal/tree/main/rev.5/5.1)

### Videos

* [Micro Journal Rev.5.1 Features and Use Cases](https://youtu.be/4LoZSAb_AXQ)
* [YouTube Playlist of Micro Journal Rev.5](https://www.youtube.com/playlist?list=PLrUXYLEnAaNT9xCD-dFa0QLdjVJLV7N7T)


---

## Practical info

### Ordering from Korea? / 한국에서 주문하시는 경우

한국에서 주문을 고려하시는 경우 아래 사이트에 이메일 등록해주시면, 제작 가능할 때, 바로 연락을 드리겠습니다.

If you are considering an order from Korea, please register your email at the site below. I will contact you as soon as a unit is available to build.

[https://www.yesbut.it/sales](https://www.yesbut.it/sales)

### In case of repair request

If your unit needs a repair, I can have a look, fix it and ship it back. In the worst case, I can build a new one and ship it to you, provided that you send me the unit and cover the shipping cost and the material cost (if necessary). Shipping it back typically costs me around 55 USD. Send me a message when you need a repair.

### User Reviews

* [+1 for the Micro Journal](https://www.reddit.com/r/writerDeck/comments/1cyvjsf/1_for_the_micro_journal/)
* [My first WriterDeck - the Micro Journal Rev 5.](https://www.reddit.com/r/writerDeck/comments/1cytyq6/my_first_writerdeck_the_micro_journal_rev_5/)

### Press

* [Hackster.io: Micro Journal Offers a Customizable writerDeck Experience](https://www.hackster.io/news/micro-journal-offers-a-customizable-writerdeck-experience-4ffbf773f3ec)
* [Guest Starring at Canadian National Radio](https://ici.radio-canada.ca/nouvelle/2080542/telephone-idiot-minimaliste-dumbphone)
* [Pascal Forget: Micro Journal – machine à écrire](https://www.pascalforget.com/micro-journal/)
* [98.5 Canadian Radio Channel about Micro Journal](https://www.985fm.ca/audio/632913/un-clavier-ergonomique-ideal-pour-le-teletravail)
* [Tindie Blog: Micro Journal DIY Kit](https://blog.tindie.com/2024/11/micro-journal-diy-kit/)

### Community

* [Un Kyu Lee's Design Gallery](https://www.yesbut.it/)
* [YouTube – @unkyulee](https://www.youtube.com/@unkyulee)
* [Reddit – Un Kyu Lee](https://www.reddit.com/r/unkyulee/)
* [Reddit – writerDeck](https://www.reddit.com/r/writerDeck/)
* [Micro Journal Rev.5 Discussion Forum](https://www.flickr.com/groups/alphasmart/discuss/72157721921183163/)
