#include "BluetoothConfig.h"
#include "../Menu.h"
#include "app/app.h"
#include "app/config/config.h"
#include "service/display/display.h"
#include "screens/WordProcessor/WordProcessor.h"

//
void BluetoothConfig_setup(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    // when entering the screen
    // clear the screen
    Menu_clear();
}

//
void BluetoothConfig_render(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    ptft->setCursor(0, 30, 2);
    ptft->setTextSize(1);

    // Bluetooth Keyboard Config
    ptft->println(" Bluetooth Keyboard Setup ");

    //
    ptft->println();
    ptft->println(" [B] BACK ");
}

//
void BluetoothConfig_keyboard(char key)
{
    JsonDocument &app = app_status();

    // go back to home menu
    //
    app["menu"]["state"] = MENU_HOME;
}
