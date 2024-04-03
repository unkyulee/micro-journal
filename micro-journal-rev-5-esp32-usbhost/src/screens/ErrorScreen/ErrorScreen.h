#ifndef ErrorScreen_h
#define ErrorScreen_h

#include <TFT_eSPI.h>

//
void ErrorScreen_setup(TFT_eSPI* ptft);

// 
void ErrorScreen_render(TFT_eSPI* ptft);

// 
void ErrorScreen_keyboard(char key);

#endif