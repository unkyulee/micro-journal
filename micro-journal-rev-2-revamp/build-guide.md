DRAFT

# Build Guide

This guide is for building the **Micro Journal Rev.2.ReVamp**, allowing you to assemble one on your own. It provides detailed information for troubleshooting if any parts of the build encounter issues and serves as a resource for those curious about the construction of the Micro Journal Rev.2.ReVamp.

To complete this build, basic soldering skills are necessary, though advanced skills aren’t required. All components are through-hole types, so with some practice, you’ll be able to achieve the level needed.

You’ll also need access to 3D-printed parts to complete the build. Many 3D printing services are available, but if you’d prefer to assemble your own and are having difficulty sourcing components, feel free to contact me—I’ll do my best to help provide a DIY kit.

[Buy a DIY Kit] TBD

# Materials

- [Wisecoco 7.84 Inch 1280x400 LCD Display](https://www.aliexpress.com/item/1005004986951553.html)
- Raspberry Pi Zero 2W
- Raspberry Pi Pico (rp2040)
- [48 Keyboard PCB](https://www.tindie.com/products/unkyulee/micro-journal-diy-kit-keyboard-pcb/)
- [O-ring 10x5.2x2.4](https://www.aliexpress.com/item/1005002753756030.html)
- [Lipo Battery Charger and Power Supply Module](https://www.aliexpress.com/item/1005006366996657.html)
- EC11 15mm Half handle
- 48x Diode IN4148
- 48x Kailih Hot-Swappable PCB socket Cherry MX Switches (CPG151101S11)
- 2x [Micro USB cable - 2 pin USB male](https://www.aliexpress.com/item/1005004982855996.html) 
- 1x [Micro USB cable - 4 pin Micro Male](https://www.aliexpress.com/item/1005004982855996.html) 
- Micro SD card minimum 4 GB
- 2x [FPV mini HDMI connector](https://www.aliexpress.com/item/1005004374555360.html)
- [FFC HDMI Cable](https://www.aliexpress.com/item/1005004374555360.html)

To be completed with further detailed list of materials

# Build Order

1. Prepare the Enclosure
2. Build Keyboard PCB
3. Power Supply
4. Display and Rasbperry Pi 2W
5. Closing the Enclosure
6. Switches and Keycaps
7. Flashing the SD card

# Enclosure Preparation

[STL files](https://github.com/unkyulee/micro-journal/tree/main/micro-journal-rev-2-revamp/STL) for the 3D prints are available in the github.

Please, refer to the image below for placement of the prints. Most of the prints requries support. I have used tree support for the best results. Bolts do not need supports. I have used PLA for the print material.

<img src="build/build_001.png" width="400">

Once the prints are done. Remove the support materials from the print.

# Keyboard PCB

[Gerber File](https://github.com/unkyulee/micro-journal/tree/main/micro-journal-rev-6-one-piece/PCB) for the Keyboard PCB are available in the github.

You can print and order the PCB using the PCB manufacturing services. Typically I use JLCPCB or PCBWay for the order. They will ask for minimum quantify of 5 PCBs. So you will have extra PCB. These PCBs can be reused for other projects that needs mini mechanical keyboard, so, it's not too bad to have some extra laying around.

I think Keyboard PCB is the only component that can't be sourced from the market. Apart from the 3d printed materials. I have prepared a kit packed with PCB and diode and hotswappable sockets. If ordering from PCB manufacturer is found a bit difficult, you can use my tindie shop to order a PCB parts.

[Buy Keyboard PCB DIY Kit](https://www.tindie.com/products/unkyulee/micro-journal-diy-kit-keyboard-pcb/)

In order to assemble the keyboard PCB. You will need 48 pieces of the following items.

- Kailh Hot-swappable PCB socket Hot Plug
- IN4148 diode. Through hole size.

<img src="../micro-journal-rev-6-one-piece/build/pcb_001.png" width="600">

First step is to apply solder to the PCB, where hot socket will be placed. Place the tip of the soldering iron on top of squared copper pad on the PCB, then melt the solder on it. Apply it slightly more than usual. Until you feel like there is a little blob of solder on it. It will be easier to place the hot-swappable socket.

<img src="../micro-journal-rev-6-one-piece/build/pcb_002.png" width="600">

Once both slots are pre soldered. Place the hotswappable socket on top of it. There should be a hole that fits into the PCB, so should be quite easy to place them. Once there are on the right place. Place the soldering iron on the side where you have presoldered, you will feel the solder melting and the socket sinking into the hole. Try to use some wooden stick not to get burned. Apply the solder on both ends of the socket.

Do this for 48 times.

<img src="../micro-journal-rev-6-one-piece/build/pcb_003.png" width="600">

Now it is time to place the diodes. Place the diode head on the side where there are the sockets. Place the black line aligned with the directions that are printed on the PCB. Diodes has polarity. So, you must make sure that the direction that you place the diodes are correct.

<img src="../micro-journal-rev-6-one-piece/build/pcb_004.png" width="600">

When all diodes are in place. Solder their legs, and cut of to the excess legs.

Do this for 48 times.

# Rasbperry Pi Pico and Keyboard PCB Wiring

[TBD Image of Raspberry Pi Pico]

<img src="../micro-journal-rev-6-one-piece/build/esp32_002.png" width="600">

| Keyboard PIN No. | RP Pico PIN No. |
| ---------------- | --------------- |
| 1                | 2               |
| 2                | 3               |
| 3                | 4               |
| 4                | 5               |
| 5                | 6               |
| 6                | 7               |
| 7                | 8               |
| 8                | 9               |
| 9                | 10              |
| 10               | 11              |
| 11               | 12              |
| 12               | 13              |
| 13               | 14              |
| 14               | 15              |
| 15               | 16              |
| 16               | 17              |

Rotary encoder EC11 will have two pins for the button, and 3 pins for the encoder. 

One button pin will have diode negative end connected to GP18. The other button pin connected to Keyboard PCB pin 5 (RP GPIO 6) for the left, and Keyboard PCB pin 16 (RP GPIO 17) for the right. 

Encoder pins GP19, GP20 for the left encoder. 
GP21, and GP22 for the right encoder. 
Middle pin on the encoder is connected to GND of PICO

[Picture of wiring to be included]


# Power Supply Module

Need two Micro USB cable for power (just two cables)

Need one Micro USB cables for Keyboard connection (four cables)

Green for the TP2, White for TP3

wiring picture to be included




# Display and Raspberry Pi Zero 2W

Display should have controller board. Controller board glued to the display back plate. 

Connect the interface cable to the controller board to the display.

Go through the 3d printed enclosure hole 3 usb cables

Create FPV HDMI cable and connector

