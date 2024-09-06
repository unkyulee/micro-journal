#ifndef WakeUp_h
#define WakeUp_h

#include <TFT_eSPI.h>
#include <U8g2_for_TFT_eSPI.h>
#include <AnimatedGIF.h>

#include <SPI.h>
#include <SPIFFS.h>
#include <SD.h>

#define NORMAL_SPEED // Comment out for rame rate for render speed test


// GIFDraw is called by AnimatedGIF library frame to screen
#define BUFFER_SIZE 320 // Optimum is >= GIF width or integral division of width

//
void WakeUp_setup(TFT_eSPI* ptft, U8g2_for_TFT_eSPI *pu8f, bool wakeup);

// 
void WakeUp_render(TFT_eSPI* ptft, U8g2_for_TFT_eSPI *pu8f);

// 
void WakeUp_keyboard(char key);

// 
void WakeUp_load_gif(bool wakeup);
void WakeUp_free_gif();
bool WakeUp_load_gif_sd(String filename);
bool WakeUp_load_gif_spiffs(String filename);

//
void GIFDraw(GIFDRAW *pDraw);

#endif