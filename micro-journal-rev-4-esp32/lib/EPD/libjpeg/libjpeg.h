
#ifndef LIB_LPEG_H
#define LIB_LPEG_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/***        include files                                                   ***/
/******************************************************************************/

#include "epd_driver.h"

#include <stdint.h>

/******************************************************************************/
/***        macro definitions                                               ***/
/******************************************************************************/

/**
 * @brief Adds dithering to image rendering (Makes grayscale smoother on transitions)
 */
#define JPG_DITHERING 0

/**
 * @brief Measure the time consumed by jpeg image processing.
 */
#define LIBJPEG_MEASURE 1

/******************************************************************************/
/***        type definitions                                                ***/
/******************************************************************************/

/******************************************************************************/
/***        exported variables                                              ***/
/******************************************************************************/

/******************************************************************************/
/***        exported functions                                              ***/
/******************************************************************************/

void libjpeg_init(void);
void show_jpg_from_buff(uint8_t *buff, uint32_t buff_size, Rect_t area);
// void show_jpg_from_spiffs(const char *fn);
void libjpeg_deinit(void);

#ifdef __cplusplus
}
#endif

#endif
/******************************************************************************/
/***        END OF FILE                                                     ***/
/******************************************************************************/