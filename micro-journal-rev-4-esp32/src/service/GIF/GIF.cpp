#include "GIF.h"
#include "app/app.h"
#include "display/display.h"
#include "keyboard/keyboard.h"

//
#include <AnimatedGIF.h>

#ifdef USE_DMA
uint16_t usTemp[2][GIF_BUFFER_SIZE]; // Global to support DMA use
#else
uint16_t usTemp[1][GIF_BUFFER_SIZE]; // Global to support DMA use
#endif
bool dmaBuf = 0;

//
// Load GIF library
AnimatedGIF gif;
void GIFDraw(GIFDRAW *pDraw);

// global for GIFDraw to reference
TFT_eSPI *ptft;

//
uint8_t *gif_image = NULL;
size_t gif_image_size = 0;

//
bool stop_gif = false;
bool play_once = false;
int screen_next;

//
bool gif_setup(TFT_eSPI *p, U8g2_for_TFT_eSPI *pu8f, String filename, int screenNext, bool playOnce)
{
    //
    gif_unload();

    // save the display handle
    ptft = p;

    //
    gif.begin(BIG_ENDIAN_PIXELS);

    // reset stop flag
    stop_gif = false;
    play_once = playOnce;
    screen_next = screenNext;

    // load gif image
    _log("[gif_setup] GIF setup completed and loaded file: %s\n", filename.c_str());
    return gif_load(filename);
}

void gif_unload()
{
    if (gif_image != NULL)
    {
        free(gif_image);
        _log("Free gif\n");
        gif_image = NULL;
    }
}


//
void gif_render(TFT_eSPI *ptft, U8g2_for_TFT_eSPI *pu8f)
{
    int delayMs = 0;

    if (gif.open((uint8_t *)gif_image, gif_image_size, GIFDraw))
    {
        ptft->startWrite();

        while (!stop_gif && gif.playFrame(false, &delayMs, NULL))
        {
            unsigned long start = millis();

            // loop until this frame’s delay expires, but bail early if stop requested
            while (!stop_gif && (millis() - start < (unsigned long)delayMs))
            {
                yield();  // let ESP32 handle WiFi, etc.
            }
        }

        gif.close();
        ptft->endWrite();
    }

    // just play once
    if (play_once || stop_gif)
    {
        // free memory
        gif_unload();

        // move to next screen
        JsonDocument &app = status();
        app["screen"] = screen_next;
    }
}

//
bool gif_load(String filename)
{
    // check if file exists
    _log("[gif_load] checking if file exists: %s\n", filename.c_str());
    if (gfs()->exists(filename.c_str()))
    {
        _log("[gif_load] Loading %s from the file system\n", filename.c_str());
        // load image
        File file = gfs()->open(filename.c_str(), "r");
        if (!file)
        {
            _log("[gif_load] Failed to open file for reading\n");
            return false;
        }

        // Allocate memory for the image
        gif_image_size = file.size();

        // limit the gif size to 1MB
        if (gif_image_size > 1100000)
        {
            _log("[gif_load] File size too big %d from SD\n", gif_image_size);
            return false;
        }

        //
        _log("[gif_load] File size: %d\n", gif_image_size);
        gif_image = (uint8_t *)malloc(gif_image_size);
        if (gif_image == NULL)
        {
            _log("[gif_load] Failed to allocate memory\n");
            return false;
        }
        _log("[gif_load] File alloc success\n");

        // laod gif to the memory
        size_t bytesRead = file.readBytes((char *)gif_image, gif_image_size);
        _log("[gif_load] File read: %d\n", bytesRead);

        // Close the file
        file.close();
        delay(100);

        // gif loading success
        return true;
    }

    else
    {
        _log("[gif_load] file doesn't not exist: %s\n", filename.c_str());
        return false;
    }
}

//
void gif_stop(int _screen_next)
{
    //
    if (_screen_next != -1)
        screen_next = _screen_next;

    //
    _log("Stop GIF animation6 to move to next screen %d\n", screen_next);

    // at the next frame gif will stop
    stop_gif = true;
}

// Draw a line of image directly on the LCD
void GIFDraw(GIFDRAW *pDraw)
{
    if (stop_gif) return; // abort drawing this frame

    //
    uint8_t *s;
    uint16_t *d, *usPalette;
    int x, y, iWidth, iCount;

    // Display bounds check and cropping
    iWidth = pDraw->iWidth;
    if (iWidth + pDraw->iX > ptft->width())
        iWidth = ptft->width() - pDraw->iX;
    usPalette = pDraw->pPalette;
    y = pDraw->iY + pDraw->y; // current line
    if (y >= ptft->height() || pDraw->iX >= ptft->width() || iWidth < 1)
        return;

    // Old image disposal
    s = pDraw->pPixels;
    if (pDraw->ucDisposalMethod == 2) // restore to background color
    {
        for (x = 0; x < iWidth; x++)
        {
            if (s[x] == pDraw->ucTransparent)
                s[x] = pDraw->ucBackground;
        }
        pDraw->ucHasTransparency = 0;
    }

    // Apply the new pixels to the main image
    if (pDraw->ucHasTransparency) // if transparency used
    {
        uint8_t *pEnd, c, ucTransparent = pDraw->ucTransparent;
        pEnd = s + iWidth;
        x = 0;
        iCount = 0; // count non-transparent pixels
        while (x < iWidth)
        {
            c = ucTransparent - 1;
            d = &usTemp[0][0];
            while (c != ucTransparent && s < pEnd && iCount < GIF_BUFFER_SIZE)
            {
                c = *s++;
                if (c == ucTransparent) // done, stop
                {
                    s--; // back up to treat it like transparent
                }
                else // opaque
                {
                    *d++ = usPalette[c];
                    iCount++;
                }
            } // while looking for opaque pixels
            if (iCount) // any opaque pixels?
            {
                // DMA would degrade performance here due to short line segments
                ptft->setAddrWindow(pDraw->iX + x, y, iCount, 1);
                ptft->pushPixels(usTemp, iCount);
                x += iCount;
                iCount = 0;
            }
            // no, look for a run of transparent pixels
            c = ucTransparent;
            while (c == ucTransparent && s < pEnd)
            {
                c = *s++;
                if (c == ucTransparent)
                    x++;
                else
                    s--;
            }
        }
    }
    else
    {
        s = pDraw->pPixels;

        // Unroll the first pass to boost DMA performance
        // Translate the 8-bit pixels through the RGB565 palette (already byte reversed)
        if (iWidth <= GIF_BUFFER_SIZE)
            for (iCount = 0; iCount < iWidth; iCount++)
                usTemp[dmaBuf][iCount] = usPalette[*s++];
        else
            for (iCount = 0; iCount < GIF_BUFFER_SIZE; iCount++)
                usTemp[dmaBuf][iCount] = usPalette[*s++];

#ifdef USE_DMA // 71.6 fps (ST7796 84.5 fps)
        tft.dmaWait();
        tft.setAddrWindow(pDraw->iX, y, iWidth, 1);
        tft.pushPixelsDMA(&usTemp[dmaBuf][0], iCount);
        dmaBuf = !dmaBuf;
#else // 57.0 fps
        ptft->setAddrWindow(pDraw->iX, y, iWidth, 1);
        ptft->pushPixels(&usTemp[0][0], iCount);
#endif

        iWidth -= iCount;
        // Loop if pixel buffer smaller than width
        while (iWidth > 0)
        {
            // Translate the 8-bit pixels through the RGB565 palette (already byte reversed)
            if (iWidth <= GIF_BUFFER_SIZE)
                for (iCount = 0; iCount < iWidth; iCount++)
                    usTemp[dmaBuf][iCount] = usPalette[*s++];
            else
                for (iCount = 0; iCount < GIF_BUFFER_SIZE; iCount++)
                    usTemp[dmaBuf][iCount] = usPalette[*s++];

#ifdef USE_DMA
            tft.dmaWait();
            tft.pushPixelsDMA(&usTemp[dmaBuf][0], iCount);
            dmaBuf = !dmaBuf;
#else
            ptft->pushPixels(&usTemp[0][0], iCount);
#endif
            iWidth -= iCount;
        }
    }
} /* GIFDraw() */