# Micro Journal Rev.3: Nadia

## User Guide

### Cable Characteristics

There is a USB port located below the keyboard PCB. This connects to a Raspberry Pi Pico, which functions as the keyboard's microcontroller. When connected to a PC or phone, the device operates as a standard keyboard.

This connection does **not** support USB Power Delivery (PD). Because of this, you must use a **USB-A to USB-C data cable**. A USB-C to USB-C cable will not work in most cases, as the host device cannot establish proper communication with the Pico.

#### Connecting to a PC

Use a USB-A to USB-C cable:

* USB-C end → keyboard
* USB-A end → PC

#### Connecting to a USB-C Phone

Use a USB-A to USB-C cable together with an OTG adapter:

* USB-C end → keyboard
* USB-A end → OTG adapter
* OTG adapter → phone

This allows the phone to recognize the keyboard correctly.

#### iPhone Compatibility

Some iPhones may not recognize this setup. In such cases, you may need to use a **Lightning/USB-C camera adapter** to establish a proper connection.

#### Charging While in Use

If you want to use the keyboard while charging your phone, use a **USB hub with power input**. This allows simultaneous connection and charging.

---

### Keymap

The keyboard has 48 keys and two knobs. To fit everything on 48 keys, the keymap has four **layers**. A layer is a full set of key assignments, and the keyboard switches layers while you hold a layer key:

| Layer | How to get it | What is on it |
| ----- | ------------- | ------------- |
| **Base** | No layer key held | Letters, Space, Enter, arrow keys |
| **Lower** | Hold `Lower` | Shifted symbols `! @ # $ ...`, Delete, Home / End |
| **Raise** | Hold `Raise` | Numbers, F1 to F12, `- = [ ]` |
| **Nav** | Hold `Lower` and `Raise` together | Home / End and Page Up / Down on the knobs, bootloader key |

Release the layer key and you're back on Base. A key left empty in the tables below works the same as on Base.

A printable version with diagrams is in [keymap.pdf](../../shared/keyboard-48/rp2040/keymap.pdf).

#### Base

| | | | | | | | | | | | |
|---|---|---|---|---|---|---|---|---|---|---|---|
| Esc | Q | W | E | R | T | Y | U | I | O | P | Bksp |
| \\ | A | S | D | F | G | H | J | K | L | ; | ' |
| Shift | Z | X | C | V | B | N | M | , | . | / | Shift / Enter |
| Ctrl | Tab | Alt | GUI | **Lower** | Space | Space | **Raise** | ← | ↓ | ↑ | → |

`Shift / Enter`: tap for **Enter**, hold for **Right Shift**.

#### Lower (hold `Lower`)

| | | | | | | | | | | | |
|---|---|---|---|---|---|---|---|---|---|---|---|
| ~ | ! | @ | # | $ | % | ^ | & | * | ( | ) | Del |
| | | | | | | | _ | + | { | } | \| |
| | | | | | | | | | | | |
| | | | | *(held)* | | | | Home | PgDn | PgUp | End |

#### Raise (hold `Raise`)

| | | | | | | | | | | | |
|---|---|---|---|---|---|---|---|---|---|---|---|
| ` | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 0 | |
| | F1 | F2 | F3 | F4 | F5 | F6 | - | = | [ | ] | |
| | F7 | F8 | F9 | F10 | F11 | F12 | | | | | |
| | | | | | | | *(held)* | Home | PgDn | PgUp | End |

#### Nav (hold `Lower` + `Raise`)

| | | | | | | | | | | | |
|---|---|---|---|---|---|---|---|---|---|---|---|
| | Boot | | | | | | | | | | |
| | | | | | | | | | | | |
| | | | | | | | | | | | |
| | | | | *(held)* | | | *(held)* | Home | PgDn | PgUp | End |

`Boot` restarts the Pico in firmware-update mode (see [Updating the Firmware](#updating-the-firmware)).

#### Knobs

Turning a knob sends a different key depending on the layer:

| Layer | Left knob (counter-clockwise / clockwise) | Right knob (counter-clockwise / clockwise) |
| ----- | ----------------------------------------- | ------------------------------------------ |
| Base | Left arrow / Right arrow | Up arrow / Down arrow |
| Lower | Mouse left / Mouse right | Mouse up / Mouse down |
| Raise | Scroll up / Scroll down | Scroll left / Scroll right |
| Nav | Home / End | Page Up / Page Down |

Pressing a knob does the same thing on every layer: the left knob is **Space** and the right knob is **Shift / Enter**.

---

### Key Customization

You can change any key, and the knobs, with **QMK Vial**:

[https://get.vial.today/](https://get.vial.today/)

1. Open the page in Chrome or Edge (or use the Vial desktop app) and connect the keyboard with the USB cable.
2. Pick a layer tab (0 = Base, 1 = Lower, 2 = Raise, 3 = Nav), click a key, then choose the new keycode.
3. Changes are saved on the keyboard right away. You don't need to flash anything.

Before some changes Vial asks you to unlock the keyboard. When it does, hold `Esc` + `Q` until it unlocks.

Be careful when replacing `Lower` or `Raise`: without them you can't reach the other layers. Before you replace one, find its keycode in Vial so you can put it back.

### Updating the Firmware

To put the keyboard into firmware-update mode, do one of the following:

* Press `Lower` + `Raise` + `Q` (Boot)
* Hold `Esc` while plugging in the USB cable
* Hold the **BOOTSEL** button on the Pico while plugging it in

The Pico appears on your computer as a drive named `RPI-RP2`. Copy [unkyulee_vial.uf2](../../shared/keyboard-48/rp2040/unkyulee_vial.uf2) onto it. The Pico restarts by itself as a keyboard.



* This guide is currently in draft form and will be updated with additional information over time.