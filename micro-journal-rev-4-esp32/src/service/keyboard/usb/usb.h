#ifdef USBHOST

#ifndef Keyboard_USB_h
#define Keyboard_USB_h

//
#include <Arduino.h>

#define FN 28
#define SHIFT 29

// special key
#define EMPTY 0x0
#define MENU 0x6
#define BACK 0x5

//
void keyboard_setup();

///
void keyboard_loop();
static void keyboard_connected(uint8_t usbNum, void *dev);
static void keyboard_print(uint8_t usbNum, uint8_t byte_depth, uint8_t *data, uint8_t data_len);


#endif

#endif