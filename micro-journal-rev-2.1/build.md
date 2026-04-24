
# Micro Journal Rev.2.1. Build Guide

This guide is for building the **Micro Journal Rev.2.1: cyberDeck**, allowing you to assemble one on your own. It provides detailed information for troubleshooting if any parts of the build encounter issues and serves as a resource for those curious about the construction of the Micro Journal Rev.2.ReVamp.

- [Video Guide](https://youtu.be/-GmhE2VCncA)

To complete this build, basic soldering skills are necessary, though advanced skills aren't required. All components are through-hole types, so with some practice, you'll be able to achieve the level needed.

You'll also need access to 3D printed parts to complete the build. Many 3D printing services are available, but if you'd prefer to assemble your own and are having difficulty sourcing components, feel free to contact me. I'll do my best to help provide a DIY kit.

- Build Time: 4 ~ 6 hours
- Basic level of soldering is required to complete the build


# Materials

All the components are easily obtainable from Amazon, or Aliexpress. You should be able to identify the exact item looking at the build guide video. 
One component may not come in easily is the Keyboard PCB. There is a link to the  where you can buy just one or low quanity directly from the supplier.

Most components are readily available from platforms such as Amazon or AliExpress. You should be able to identify the exact items by referring to the build guide video. One component that may be slightly more difficult to source is the [keyboard PCB](https://www.elecrow.com/micro-journal-diy-kit-68-keys-keyboard-pcb.html). You can purchase it [directly from the supplier](https://www.elecrow.com/micro-journal-diy-kit-68-keys-keyboard-pcb.html), even in small quantities

- [Wisecoco 7.84 Inch 1280x400 LCD Display](https://www.aliexpress.com/item/1005004986951553.html)
- [Rubber O-ring 18x13.2x2.4mm](https://www.aliexpress.com/item/1005002753756030.html)
- [HDMI Cable 25 cm](https://www.aliexpress.com/item/1005007721692972.html)

- Raspberry Pi Zero 2W
- Raspberry Pi Pico (rp2040)
- Micro SD card minimum 8 GB

- [68 Keyboard PCB](https://www.elecrow.com/micro-journal-diy-kit-68-keys-keyboard-pcb.html)
- 2x [EC11 15mm Half handle](https://it.aliexpress.com/item/1005005983134515.html) 

- [18650 Lithium Battery Shield with 4 Battery Holder](https://www.aliexpress.com/item/1005007581124399.html)
- [Micro USB Hub with Power Input](https://www.aliexpress.com/item/1005006750076510.html)
- [2x Micro USB 2 PIN Male Cable](https://www.aliexpress.com/item/1005008243661710.html)
- [USB 2 PIN Male Cable](https://www.aliexpress.com/item/1005008243661710.html)
- SPST Snap in Switch ON Off 2Pin 19mm

- DIN 912 M3 Hex Screw Length 5mm
- DIN 912 M3 Hex Screw Length 10mm
- DIN 912 M3 Hex Screw Length 50mm
- DIN 7046 M2 Machine Screw Length 5mm

- M3 Heated Inserts OD 4.5mm Length 3mm
- M2 Heated Inserts OD 3.2mm Length 3mm

- You will need TORX T10H to handle Hex screws

- Any typical wires for electronics would do. I use [Wires 30 AWG](https://it.aliexpress.com/item/1005007081117235.html)



# 3D Prints

<img src="./images/build/001.png" width=600 />

I use PLA+ filaments to print the enclosure. For the best results, please follow the recommended part orientations when printing.

The printer I use is the Bambulab P1S. In the [STL](./STL/) folder, you will find a "rev.2.1.3mf" file, which is the complete 3D printing project file. It includes the settings I use for printing the build. Printing the full enclosure takes approximately 30 hours. It is a relatively large print, but the assembly process is very rewarding, and seeing the final result makes it well worth the effort.

The STL folder also contains all individual STL files, which you can import into your own toolchain for printing. The enclosure was designed using Fusion 360, and the original design file is included as well. You are welcome to modify the design and adjust dimensions to suit your specific needs.



# Wiring the Keyboard

<img src="./images/build/002.png" width=400 />

| Keyboard PCB PIN | Connects to           |
| ---------------- | --------------------- |
| Pin 0            | Right Knob Out B      |
| Pin 1            | Right Knob GND        |
| Pin 2            | Right Knob Out A      |
| Pin 3            | Right Knob Switch     |
| Pin 4            | Right Knob Switch GND |
|                  |                       |
| Pin 5            | PICO GPIO 2           |
| Pin 6            | PICO GPIO 3           |
| Pin 7            | PICO GPIO 4           |
| Pin 8            | PICO GPIO 5           |
| Pin 9            | PICO GPIO 6           |
| Pin 10           | PICO GPIO 7           |
| Pin 11           | PICO GPIO 8           |
| Pin 12           | PICO GPIO 9           |
| Pin 13           | PICO GPIO 10          |
| Pin 14           | PICO GPIO 11          |
| Pin 15           | PICO GPIO 12          |
| Pin 16           | PICO GPIO 13          |
| Pin 17           | PICO GPIO 14          |
| Pin 18           | PICO GPIO 15          |
| Pin 19           | PICO GPIO 16          |
| Pin 20           | PICO GPIO 17          |
| Pin 21           | PICO GPIO 18          |
| Pin 22           | PICO GPIO 19          |
| Pin 23           | PICO GPIO 20          |
| Pin 24           | PICO GPIO 21          |
| Pin 25           | PICO GPIO 22          |
| Pin 26           | PICO GND              |
|                  |                       |
| Pin 27           | Left Knob Out B       |
| Pin 28           | Left Knob GND         |
| Pin 29           | Left Knob Out A       |
| Pin 30           | Left Knob Switch      |
| Pin 31           | Left Knob Switch GND  |




