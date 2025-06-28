#ifndef Display_h
#define Display_h

// Screen Type
#define WORDPROCESSOR 0

//
#define ERRORSCREEN 1
#define MENUSCREEN 2
#define WAKEUPSCREEN 3
#define SLEEPSCREEN 4
#define USBDRIVESCREEN 5

// menu id
#define MENU_HOME 0
#define MENU_SYNC 1
#define MENU_CLEAR 2
#define MENU_LAYOUT 3
#define MENU_WIFI 4
#define MENU_FIRMWARE 5 
#define MENU_RESET 6
#define MENU_BUTTONS 7
#define MENU_BACKGROUND 8
#define MENU_FOREGROUND 9
#define MENU_STARTUP 10
#define MENU_BLUETOOTH 11

//
void display_setup();
void display_loop();

//
void display_keyboard(int key);

#endif