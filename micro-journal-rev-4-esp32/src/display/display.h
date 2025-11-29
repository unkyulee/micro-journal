#pragma once

//
#include <Arduino.h>

// Screen Type
#define WORDPROCESSOR 0

//
#define ERRORSCREEN 1
#define MENUSCREEN 2
#define WAKEUPSCREEN 3
#define SLEEPSCREEN 4
#define KEYBOARDSCREEN 5
#define UPDATESCREEN 6

// menu id
#define MENU_HOME 0
#define MENU_SYNC 1
#define MENU_CLEAR 2
#define MENU_LAYOUT 3
#define MENU_WIFI 4
#define MENU_FIRMWARE 5
#define MENU_BUTTONS 7
#define MENU_BACKGROUND 8
#define MENU_FONTCOLOR 9
#define MENU_BLUETOOTH 11
#define MENU_STORAGE 12
#define MENU_BRIGHTNESS 13
#define MENU_INFO 14

// MENU button
#define FN 28

// special key
#define EMPTY 0x0
#define MENU 0x6

//
void display_setup();
void display_loop();
int display_core(); // show which core to run display routine

//
void display_keyboard(int key, bool pressed, int index = -1);
void display_keyboard_report(uint8_t modifier, uint8_t reserved, uint8_t* keycodes);

