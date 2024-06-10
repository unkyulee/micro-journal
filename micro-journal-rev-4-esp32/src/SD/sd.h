#ifndef SD_h
#define SD_h

#ifdef KEYPAD
#define SD_CS 5
#else
#define SD_CS 9
#endif


//
void SD_setup();

///
void SD_loop();

//
void SD_firwamre_update();

#endif