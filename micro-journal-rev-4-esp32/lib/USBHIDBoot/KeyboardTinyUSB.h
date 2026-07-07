#pragma once

#ifndef USE_TINYUSB
#error KeyboardTinyUSB requires -D USE_TINYUSB
#endif

#include <Arduino.h>
#include <Adafruit_TinyUSB.h>

// Boot-protocol compatible USB HID keyboard.
//
// The stock Arduino-Pico `Keyboard` library (Pico-SDK USB stack) always
// declares its HID interface as HID_ITF_PROTOCOL_NONE and prefixes every
// report with a Report ID byte. Most BIOS/UEFI "Legacy USB Keyboard
// Support" implementations only recognize keyboards that identify as a
// USB HID Boot Interface (bInterfaceSubClass=1, bInterfaceProtocol=1) and
// send the plain, unprefixed 8-byte boot report, so that keyboard never
// works at the CMOS/BIOS setup screen even though it works fine once an
// OS is loaded.
//
// This does NOT subclass the core's own HID_Keyboard library: that
// library's HID_Keyboard.cpp unconditionally #includes Keyboard.h, which
// #errors under USE_TINYUSB -- so HID_Keyboard (and anything built on
// it, including this) can't be compiled together with USE_TINYUSB via
// the framework's own class hierarchy. The press/release/ASCII-layout
// logic and KEY_* codes below are ported directly from that library
// (Arduino LGPL, Copyright (c) 2015 Arduino LLC / Earle F. Philhower,
// III) to avoid pulling HID_Keyboard.cpp into the build at all.

// Modifiers
#define KEY_LEFT_CTRL     0x80
#define KEY_LEFT_SHIFT    0x81
#define KEY_LEFT_ALT      0x82
#define KEY_LEFT_GUI      0x83
#define KEY_RIGHT_CTRL    0x84
#define KEY_RIGHT_SHIFT   0x85
#define KEY_RIGHT_ALT     0x86
#define KEY_RIGHT_GUI     0x87

// Misc keys
#define KEY_UP_ARROW      0xDA
#define KEY_DOWN_ARROW    0xD9
#define KEY_LEFT_ARROW    0xD8
#define KEY_RIGHT_ARROW   0xD7
#define KEY_BACKSPACE     0xB2
#define KEY_TAB           0xB3
#define KEY_RETURN        0xB0
#define KEY_MENU          0xED // "Keyboard Application" in USB standard
#define KEY_ESC           0xB1
#define KEY_INSERT        0xD1
#define KEY_DELETE        0xD4
#define KEY_PAGE_UP       0xD3
#define KEY_PAGE_DOWN     0xD6
#define KEY_HOME          0xD2
#define KEY_END           0xD5
#define KEY_CAPS_LOCK     0xC1
#define KEY_PRINT_SCREEN  0xCE // Print Screen / SysRq
#define KEY_SCROLL_LOCK   0xCF
#define KEY_PAUSE         0xD0 // Pause / Break

// Numeric keypad
#define KEY_NUM_LOCK      0xDB
#define KEY_KP_SLASH      0xDC
#define KEY_KP_ASTERISK   0xDD
#define KEY_KP_MINUS      0xDE
#define KEY_KP_PLUS       0xDF
#define KEY_KP_ENTER      0xE0
#define KEY_KP_1          0xE1
#define KEY_KP_2          0xE2
#define KEY_KP_3          0xE3
#define KEY_KP_4          0xE4
#define KEY_KP_5          0xE5
#define KEY_KP_6          0xE6
#define KEY_KP_7          0xE7
#define KEY_KP_8          0xE8
#define KEY_KP_9          0xE9
#define KEY_KP_0          0xEA
#define KEY_KP_DOT        0xEB

// Function keys
#define KEY_F1            0xC2
#define KEY_F2            0xC3
#define KEY_F3            0xC4
#define KEY_F4            0xC5
#define KEY_F5            0xC6
#define KEY_F6            0xC7
#define KEY_F7            0xC8
#define KEY_F8            0xC9
#define KEY_F9            0xCA
#define KEY_F10           0xCB
#define KEY_F11           0xCC
#define KEY_F12           0xCD
#define KEY_F13           0xF0
#define KEY_F14           0xF1
#define KEY_F15           0xF2
#define KEY_F16           0xF3
#define KEY_F17           0xF4
#define KEY_F18           0xF5
#define KEY_F19           0xF6
#define KEY_F20           0xF7
#define KEY_F21           0xF8
#define KEY_F22           0xF9
#define KEY_F23           0xFA
#define KEY_F24           0xFB

// ASCII layout encoding (see arduino-pico's HID_Keyboard/KeyboardLayout.h)
#define TINYUSB_KBD_SHIFT     0x80
#define TINYUSB_KBD_ALT_GR    0xc0
#define TINYUSB_KBD_ISO_KEY        0x64
#define TINYUSB_KBD_ISO_REPLACEMENT 0x32

// US ASCII -> USB HID usage code table, indexed by ASCII value.
extern const uint8_t KeyboardLayout_en_US[128];

// Low level key report: up to 6 keys and shift, ctrl etc at once
typedef struct
{
    uint8_t modifiers;
    uint8_t reserved;
    uint8_t keys[6];
} KeyReport;

class KeyboardTinyUSB_ : public Print
{
protected:
    KeyReport _keyReport;
    Adafruit_USBD_HID _hid;

    void sendReport(KeyReport *keys);

public:
    KeyboardTinyUSB_(void);
    void begin(void);
    void end(void);
    size_t write(uint8_t k);
    size_t write(const uint8_t *buffer, size_t size);
    size_t press(uint8_t k);
    size_t release(uint8_t k);
    void releaseAll(void);
};

extern KeyboardTinyUSB_ Keyboard;
