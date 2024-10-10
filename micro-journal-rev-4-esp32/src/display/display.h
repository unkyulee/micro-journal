#ifndef Display_h
#define Display_h

// Screen Type
#define WORDPROCESSOR 0
#define ERRORSCREEN 1
#define MENUSCREEN 2
#define WAKEUPSCREEN 3
#define SLEEPSCREEN 4
#define USBDRIVESCREEN 5

//
void display_setup();
void display_loop();

#endif