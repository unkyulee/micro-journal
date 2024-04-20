#include "Menu.h"
#include "app/app.h"
#include "screens/WordProcessor/WordProcessor.h"
#include "service/display/display.h"

//
#include <HTTPClient.h>
#include <SD.h>

// properties
#define MENUBAR_COLOR TFT_RED

// state
bool menu_clear = false;

// 0 - home
// 1 - sync
// 2 - delete file confirm
int menu_state = 0;

// 0 - starting
// 1 - waiting
// 2 - sending
int sync_state = 0;

// prototypes
void menu_tooolbar(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f);
void menu_home(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f);

//
void menu_sync(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f);
void menu_sync_start(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f);
void menu_sync_waiting(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f);
void menu_sync_send(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f);
void menu_sync_completed(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f);

void menu_delete(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f);

//
void menu_sync_operation();

//
void Menu_setup(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    // refresh the background
    menu_clear = true;
}

void Menu_render(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    // clear background when necessary
    if (menu_clear)
    {
        //
        ptft->fillScreen(TFT_BLACK);

        // toolbr
        ptft->fillRect(0, 0, 320, 20, MENUBAR_COLOR);
        menu_clear = false;
    }

    // display tool bar
    menu_tooolbar(ptft, pu8f);

    //
    if (menu_state == 0)
    {
        menu_home(ptft, pu8f);
    }
    else if (menu_state == 1)
    {
        menu_sync(ptft, pu8f);
    }
    else if (menu_state == 2)
    {
        menu_delete(ptft, pu8f);
    }
}

//
void Menu_keyboard(char key)
{
    // clear background for every key stroke
    menu_clear = true;

    //
    JsonDocument &app = app_status();

    // based on the current menu state
    if (menu_state == 0)
    {
        // home
        if (key == 'S' || key == 's')
        {
            if (app["config"]["sync"]["url"].as<String>().isEmpty() == false)
            {
                // to sync
                menu_state = 1;
                sync_state = 0;

                //
                app["network"]["enabled"] = true;
            }
        }

        else if (key == 'D' || key == 'd')
        {
            // to delete
            menu_state = 2;
        }

        else if (key == 'B' || key == 'b')
        {
            // go back to the word processor
            app["screen"] = WORDPROCESSOR;
        }
    }

    else if (menu_state == 1)
    {
        if (key == '\b' || key == 'b' || key == 'B')
        {
            //
            app["network"]["enabled"] = false;

            //
            sync_state = -1;
        }

        //
        menu_state = 0;

        return;
    }

    else if (menu_state == 2)
    {
        // delete confirmed
        if (key == 'Y' || key == 'y')
        {
            // empty the file
            WordProcessor::getInstance(nullptr, nullptr).emptyFile();

            // go back to the word processor
            app["screen"] = WORDPROCESSOR;
        }

        // go back to home menu
        menu_state = 0;
    }
}

// draw toolbar
void menu_tooolbar(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    ptft->setCursor(0, 2, 2);
    ptft->setTextColor(TFT_WHITE, MENUBAR_COLOR);
    ptft->setTextSize(1);
    ptft->print(" MENU ");
}

//
void menu_home(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    JsonDocument &app = app_status();

    // Text to be displayed
    ptft->setCursor(0, 30, 2);
    ptft->setTextSize(1);

    //
    ptft->setTextColor(TFT_WHITE, TFT_BLACK);
    ptft->println();

    //
    if (app["config"]["sync"]["url"].as<String>().isEmpty() == false)
    {
        ptft->println(" (S) SYNC ");
    }

    //
    ptft->println(" (D) START NEW ");
    ptft->println();
    ptft->println(" (B) BACK ");
    ptft->println();
    ptft->println();
}

//
void menu_sync(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    //
    JsonDocument &app = app_status();

    // caulcate current sync state
    if (sync_state < 2)
    {
        // when network becomes online
        // convert the state to 1
        if (app["network"]["status"].as<int>() == 0)
        {
            sync_state = 1;
        }

        else if (app["network"]["status"].as<int>() == 1)
        {
            sync_state = 2;
        }
    }

    // when sync state changes
    // clear background
    static int sync_state_prev = -1;
    if (sync_state_prev != sync_state)
    {
        //
        menu_clear = true;

        //
        sync_state_prev = sync_state;
    }

    // header
    ptft->setCursor(0, 30, 2);
    ptft->setTextColor(TFT_WHITE, TFT_BLACK);
    ptft->println(" SYNC IN PROGRESS ");

    //
    if (sync_state == 0)
    {
        menu_sync_start(ptft, pu8f);
    }
    else if (sync_state == 1)
    {
        menu_sync_waiting(ptft, pu8f);
    }
    else if (sync_state == 2)
    {
        menu_sync_send(ptft, pu8f);
    }
    else if (sync_state == 3)
    {
        menu_sync_completed(ptft, pu8f);
    }

    ptft->println("");
    ptft->println(" press (B) to stop ");
}

void menu_sync_start(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    //
    JsonDocument &app = app_status();

    ptft->setTextColor(TFT_WHITE, TFT_BLACK);
    ptft->println(" - Starting network ...");
}

void menu_sync_waiting(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    //
    JsonDocument &app = app_status();

    ptft->setTextColor(TFT_WHITE, TFT_BLACK);
    ptft->println(" - Waiting ... ");
}

void menu_sync_send(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    //
    JsonDocument &app = app_status();

    ptft->setTextColor(TFT_WHITE, TFT_BLACK);
    ptft->println(" - Sending file to drive ... ");
    ptft->println("");

    // show ip and ssid information
    String ip = app["network"]["IP"].as<String>();
    const char *ssid = app["network"]["ssid"].as<const char *>();

    ptft->printf("SSID: %s\n", ssid);
    ptft->printf("IP: %s\n", ip);

    // send file to google drive
    String url = app["config"]["sync"]["url"].as<String>();
    app_log("Requesting sync ... \n");
    app_log("%s\n", url.c_str());

    // prepare http client
    HTTPClient http;
    http.begin(url);
    http.setFollowRedirects(HTTPC_FORCE_FOLLOW_REDIRECTS);

    // Read file in chunks and send via POST request
    File file = SD.open(FILENAME, FILE_READ);
    if (!file)
    {
        ptft->println("Failed to open file");
        http.end();
        return;
    }

    //
    http.sendRequest("POST", &file, file.size());

    // Close file
    file.close();

    // close http connection
    http.end();

    sync_state = 3;
}

void menu_sync_completed(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    ptft->setTextColor(TFT_WHITE, TFT_BLACK);
    ptft->println(" - Sync completed ");
    ptft->println("");
}

//
void menu_delete(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    ptft->setCursor(0, 30, 2);
    ptft->setTextSize(1);

    //
    ptft->setTextColor(TFT_WHITE, TFT_BLACK);
    ptft->println();
    ptft->setTextColor(TFT_WHITE, TFT_RED);
    ptft->println(" (Y) ARE YOU SURE?");

    ptft->setTextColor(TFT_WHITE, TFT_BLACK);
    ptft->println();
    ptft->println("WARNING: This action will delete all text. Make sure to sync your content before confirming to prevent loss of data.");
    ptft->println();
    ptft->println(" (B) BACK ");
}
