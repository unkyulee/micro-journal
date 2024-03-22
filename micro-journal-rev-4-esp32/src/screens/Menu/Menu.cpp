#include "Menu.h"
#include "app/app.h"

//
void Menu_setup() {}

//
void Menu_render(TFT_eSPI *ptft)
{

    // Text to be displayed
    const char *text = " MENU ";

    ///
    ptft->setCursor(0, 0, 2);
    ptft->setTextSize(2); // Adjust text size as needed
    ptft->setTextColor(TFT_WHITE, TFT_BLACK);

    // Calculate text dimensions
    uint16_t textWidth = ptft->textWidth(text);
    uint16_t textHeight = ptft->fontHeight();

    // Calculate the position to draw text in the middle of the screen
    int16_t posX = (ptft->width() - textWidth) / 2;
    int16_t posY = (ptft->height() - textHeight) / 2;

    // Draw text in the middle of the screen
    ptft->setCursor(posX, posY, 2);
    ptft->print(text);
}

//
void Menu_keyboard(char key) {}
