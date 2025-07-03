#pragma once

#include <TFT_eSPI.h>
#include <U8g2_for_TFT_eSPI.h>
#include <ArduinoJson.h>

#define NORMAL_SPEED // Comment out for rame rate for render speed test

//
bool gif_setup(
    TFT_eSPI *p, U8g2_for_TFT_eSPI *pu8f, 
    String filename,
    int screenNext, 
    bool playOnce = false
);
void gif_render(TFT_eSPI *p, U8g2_for_TFT_eSPI *pu8f);

void gif_stop(int screen_next = -1);
bool gif_load(String filename);
void gif_unload();
