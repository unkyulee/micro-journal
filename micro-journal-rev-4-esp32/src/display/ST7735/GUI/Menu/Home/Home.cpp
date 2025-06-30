#include "Home.h"
#include "../Menu.h"
#include "app/app.h"
#include "display/display.h"

//
void Home_setup(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    //
    Menu_clear();
}

//
void Home_render(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    JsonDocument &app = status();

}

//
void Home_keyboard(char key, bool pressed)
{
    //
    JsonDocument &app = status();

}