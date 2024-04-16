#ifndef Keyboard_h
#define Keyboard_h

#define USBHOST
#ifdef USBHOST
#include "usb/usb.h"
#else
#include "keypad/keypad.h"
#endif

#endif