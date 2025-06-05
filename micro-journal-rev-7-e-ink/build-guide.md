DRAFT DRAFT DRAFT THIS BUILD GUIDE IS IN PROGRESS

# Micro Journal Rev.7. Build Guide

This guide is for building the **Micro Journal Rev.7**, allowing you to assemble one on your own. It provides detailed information for troubleshooting if any parts of the build encounter issues and serves as a resource for those curious about the construction of the Micro Journal Rev.7.

To complete this build, basic soldering skills are necessary, though advanced skills aren't required. All components are through-hole types, so with some practice, you'll be able to achieve the level needed.

You'll also need access to 3D-printed parts to complete the build. Many 3D printing services are available, but if you'd prefer to assemble your own and are having difficulty sourcing components, feel free to contact me—I'll do my best to help provide a DIY kit.

- Build Time: 4 ~ 6 hours
- Basic level of soldering is required to complete the build

[Buy a DIY Kit](https://www.tindie.com/products/unkyulee/micro-journal-diy-kit-all-components/)

# System Architecture

<img src="build/000.png">

Rev.2. is a small computer. Process of the building rev.2. is very similar to building a PC. There is a computer at the center of it, the assembly consists of connecting a display, power supply and the keyboard.

### A. E-Paper Display with ESP32

This build utilizes [LILYGO T5 e-Paper](https://lilygo.cc/products/t5-4-7-inch-e-paper-v2-3) module. This board contains e-paper display and ESP32 module in a single board. 

### B. Keyboard Controller - Raspberry Pi Pico RP2040

Keyboard controller is going to provide USB interface to connect to the raspberry pi computer. In order to control the key inputs, you are going to need a "raspberry pi PICO". Please, note that this is not to be confused with "Raspberry Pi Zero 2W". PICO is a micro controller. It is a tiny processing unit that is dedicated to handle the key inputs. This component will be called as "keyboard controller" throughout the document.

### C. Keyboard PCB

[Keyboard PCB](https://github.com/unkyulee/micro-journal/tree/main/micro-journal-rev-7-e-ink/PCB/SMD) can be found here. It is using SMD components. So, it's not going to be too easy to build it by hand for most people. It is possible to order the PCB assembled from the popular PCB assembly services or you can order one from my [tindie store](https://www.tindie.com/stores/unkyulee/).  

### D. Knob - EC11

Rev.7 uses two EC11 knobs. They are wired to the keyboard PCB. 

### E. LIPO Charger and Voltage Boost Controller

Power is supplied externally from LIPO batteries. In this build 18650 batteries are used. In theory, you can pick up any LIPO batteries and use them. Lilygo T5 has battery charger included in the board itself. However, it is not possible to use it in this setup. Because, keyboard is using the USB port of the T5 and in order to have the USB keyboard properly functioing, T5 needs to produce 5V and supply to the keyboard controller. However, T5 board lacks providing power via USB port. So, additional charging and voltage boost module is used to regulate the power to the build.

# Materials

- [LILYGO T5 e-Paper](https://lilygo.cc/products/t5-4-7-inch-e-paper-v2-3)
- Raspberry Pi Pico (rp2040)
- [69 Keyboard PCB](https://www.tindie.com/products/unkyulee/micro-journal-diy-kit-keyboard-pcb/)
- Micro SD card minimum 8 GB

- 12x M3 Heated Inserts OD 4.5mm Length 3mm
- 4x M3 Hex Screw Length 20mm
- 4x M3 Hex Screw Length 10mm
- 4x M3 Hex Screw Length 70mm

- 14x M2 Heated Inserts OD 3.2mm Length 3mm
- 12x M2 Hex Screw Length 5mm
- 2x M2 HEx Screw Length 3mm

- 2x [EC11 15mm Half handle](https://it.aliexpress.com/item/1005005983134515.html) for the knobs

- You will need TORX T10H to handle Hex screws
- A Wire Wrap Hand Tool can be a life saver for a lot of tasks - WSU-30
- Any typical wires for electronics would do. I use [Wires 30 AWG](https://it.aliexpress.com/item/1005007081117235.html)


# Build Order

1. Enclosure Preparation

### Enclosure Preparation

[STL files](https://github.com/unkyulee/micro-journal/tree/main/micro-journal-rev-7-e-ink/STL) for the 3D prints are available in the github.

Please, refer to the image below for placement of the prints. Vertical placements of the enclosure parts gave the best prints results, so far in my builds. This orientation gives the most coverage of the smooth surfaces.

<img src="build/001.png" width="400">

Once the prints are done. Remove the support materials from the print.

### Installing Heat Inserts

<img src="build/002.png">

<img src="build/003.png">

<img src="build/004.png">

<img src="build/005.png">



--
Currently rev.7 build guide work is in progress. 