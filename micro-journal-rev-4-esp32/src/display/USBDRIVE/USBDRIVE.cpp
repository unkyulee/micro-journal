#include "USBDRIVE.h"
#include "app/app.h"

//
void USBDRIVE_setup(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    ptft->fillScreen(TFT_BLACK);
}

//
void USBDRIVE_render(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    ///
    ptft->setCursor(0, 30, 2);
    ptft->setTextSize(1); // Adjust text size as needed
    ptft->setTextColor(TFT_WHITE, TFT_BLACK);

    // Draw text in the middle of the screen
    ptft->setCursor(0, 0, 2);
    ptft->println(" USB DRIVE MODE ");
    ptft->println();
    ptft->println();
    ptft->println();
    ptft->println(" <-- CONNECT THIS USB PORT TO THE COMPUTER ");
    ptft->println();
    ptft->println();
    ptft->println(" X-- NOT THIS ONE ");
    ptft->println();
    ptft->println();
    ptft->println(" RESTART TO BACK TO EDIT MODE");
}

//
void USBDRIVE_keyboard(char key)
{
}