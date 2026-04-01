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

### Key Customization

You can customize keymaps using **QMK Vial**:

[https://get.vial.today/](https://get.vial.today/)



* This guide is currently in draft form and will be updated with additional information over time.