#ifndef Keyboard_USB_h
#define Keyboard_USB_h

#define FN 28
#define SHIFT 0x02

// special key
#define EMPTY 0x0
#define MENU 0x6

void keyboard_setup();
void keyboard_loop();
void keyboard_key(char key);

#endif
