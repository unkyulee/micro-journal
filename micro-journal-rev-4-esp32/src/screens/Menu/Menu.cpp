#include "Menu.h"
#include "app/app.h"
#include "screens/WordProcessor/WordProcessor.h"
#include "service/display/display.h"

//
#include <HTTPClient.h>
#include <SD.h>

//
void Menu_setup(TFT_eSPI *ptft)
{
    ptft->fillScreen(TFT_BLACK);
}

//
bool clear_menu = false;
bool delete_confirm = false;
bool sync_started = false;
void Menu_render(TFT_eSPI *ptft)
{
    if (clear_menu)
    {
        ptft->fillScreen(TFT_BLACK);
        clear_menu = false;
    }

    if (sync_started)
    {
        Menu_sync(ptft);
    }
    else
    {
        JsonDocument &app = app_status();

        // Text to be displayed
        ptft->setCursor(0, 30, 2);

        //
        ptft->setTextColor(TFT_WHITE, TFT_BLACK);
        ptft->println(" SELECT AN OPERATION ");
        ptft->println();

        //
        if (app["config"]["sync"]["url"].as<String>().isEmpty() == false)
        {
            ptft->println(" (S) SYNC ");
        }

        //
        ptft->println(" (D) DELETE ALL ");

        ptft->println();
        ptft->println(" (B) BACK ");
        ptft->println();
        ptft->println();

        //
        if (delete_confirm)
        {
            ptft->setTextColor(TFT_WHITE, TFT_RED);
            ptft->println("(Y) ARE YOU SURE TO DELETE ALL?");
        }
    }
}

//
// -1: not started
// 0: network start
// 1: waiting for the network

int status_prev = -1;
int status = 0;
int response;
void Menu_sync(TFT_eSPI *ptft)
{
    //
    JsonDocument &app = app_status();

    //
    if (status_prev != status)
    {
        ptft->fillScreen(TFT_BLACK);
        status_prev = status;
    }

    // Text to be displayed
    ptft->setCursor(0, 30, 2);

    //
    ptft->setTextColor(TFT_WHITE, TFT_BLACK);
    ptft->println(" Sync Process ");
    ptft->println();

    // display network information
    if (app["network"]["status"].as<int>() == 1)
    {
        // network is connected
        // move to the next step
        if (status == 1)
            status = 2;

        //
        String ip = app["network"]["IP"].as<String>();
        const char *ssid = app["network"]["ssid"].as<const char *>();

        ptft->printf("SSID: %s\n", ssid);
        ptft->printf("IP: %s\n", ip);
        ptft->println();
    }

    // turn on the network
    if (status == 0)
    {
        ptft->println(" - Starting network ...");
        app["network"]["enabled"] = true;
        status = 1;
        response = 0;
    }

    // wait for the network to connect
    if (status == 1)
    {
        ptft->println(" - Waiting for the network ...");
    }

    // network is connected
    // perform sync
    if (status == 2)
    {
        // check if configuration exists
        if (!app["config"]["sync"].containsKey("url"))
        {
            ptft->println(" - Sync url missing");
            return;
        }
        else
        {
            //
            ptft->println(" - Sync starting ...");
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
            response = http.sendRequest("POST", &file, file.size());
            app_log("Response: %d\n", response);

            // Close file
            file.close();

            // close http connection
            http.end();

            // update the status
            status = 3;
        }
    }

    if (response > 0)
    {
        ptft->println("Sync Completed.");
    }

    ptft->println("");
    ptft->println(" press (x) to stop ");
}

void back()
{
    // go back to wordprocessor
    JsonDocument &app = app_status();
    app["screen"] = WORDPROCESSOR;
}

void stop_sync()
{
    // stop network
    JsonDocument &app = app_status();
    app["network"]["enabled"] = false;

    //
    sync_started = false;
    status_prev = 0;
    status = 0;

    //
    back();
}

//
void Menu_keyboard(char key)
{
    // every key will refresh the screen
    clear_menu = true;

    // sync
    if (sync_started)
    {
        if (key == 'x')
            stop_sync();
        return;
    }

    // delete confirm
    if (delete_confirm)
    {
        if (key == 'Y' || key == 'y')
        {
            WP_empty_file();
            back();
        }
        else
        {
            back();
        }

        delete_confirm = false;

        return;
    }

    //
    if (key == 'D' || key == 'd')
    {
        delete_confirm = true;
    }
    else if (key == 'S' || key == 's')
    {
        sync_started = true;
    }
    else if (key == 'b')
    {
        // go back to wordprocessor
        back();
    }
}
