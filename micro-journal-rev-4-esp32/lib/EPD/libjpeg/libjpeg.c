

/******************************************************************************/
/***        include files                                                   ***/
/******************************************************************************/

#include "libjpeg.h"

#include "esp_heap_caps.h"
#include "esp_log.h"
#include "esp_spiffs.h"
#include "esp_task_wdt.h"
#include "esp_timer.h"

// JPG decoder
#if ESP_IDF_VERSION_MAJOR >= 4 // IDF 4+
#include "esp32/rom/tjpgd.h"
#else // ESP32 Before IDF 4.0
#include "rom/tjpgd.h"
#endif

#include <stdio.h>
#include <string.h>
#include <math.h>

/******************************************************************************/
/***        macro definitions                                               ***/
/******************************************************************************/

/**
 * @brief Return the minimum of two values a and b
*/
#define minimum(a, b) (((a) < (b)) ? (a) : (b))

#define EP_WIDTH EPD_WIDTH

#define EP_HEIGHT EPD_HEIGHT

/******************************************************************************/
/***        type definitions                                                ***/
/******************************************************************************/

/******************************************************************************/
/***        local function prototypes                                       ***/
/******************************************************************************/

#if JPG_DITHERING
static uint8_t find_closest_palette_color(uint8_t oldpixel);
#endif

/**
 * @brief Decode and paint onto the Epaper screen
 */
static void jpeg_render(Rect_t area);

static void epd_draw_pixel_area(int x, int y, uint8_t color, uint8_t *framebuffer, Rect_t area);

/**
 * @brief 用户定义的从输入流读取数据的输入函数。
 * 
 * @param jd   Decompressor object of current session
 * @param buff Pointer to buffer to store the read data
 * @param nd   Number of bytes to read
 */
static uint32_t feed_buffer(JDEC *jd, uint8_t *buff, uint32_t nd);

/**
 * @brief User defined call-back function to output decoded RGB bitmap in
 *        decoded_image buffer
 *
 * @param jd     Decompressor object of current session
 * @param bitmap Bitmap data to be output
 * @param rect   Rectangular region to output
 */
static uint32_t tjd_output(JDEC *jd, void *bitmap, JRECT *rect);

/**
 * @brief This function opens jpeg_buf Jpeg image file and primes the decoder
 */
static int draw_jpeg(uint8_t *jpeg_buf, Rect_t *area);

/******************************************************************************/
/***        exported variables                                              ***/
/******************************************************************************/

/******************************************************************************/
/***        local variables                                                 ***/
/******************************************************************************/

const char *jd_errors[] = {
    "Succeeded",
    "Interrupted by output function",
    "Device error or wrong termination of input stream",
    "Insufficient memory pool for the image",
    "Insufficient stream input buffer",
    "Parameter error",
    "Data format error",
    "Right format but not supported",
    "Not supported JPEG standard"
};

// Affects the gamma to calculate gray (lower is darker/higher contrast)
// Nice test values: 0.9 1.2 1.4 higher and is too bright
static double gamma_value = 0.9;

static uint32_t jpeg_buf_pos;

static uint8_t *decoded_image;   // RAW decoded image
static uint8_t tjpgd_work[4096]; // tjpgd 4Kb buffer

#if LIBJPEG_MEASURE
static uint32_t time_epd_fullclear = 0;
static uint32_t time_decomp = 0;
static uint32_t time_update_screen = 0;
static uint32_t time_render = 0;
#endif

static uint8_t gamme_curve[256];

const char *TAG = "JPEG";

/******************************************************************************/
/***        exported functions                                              ***/
/******************************************************************************/

void libjpeg_init(void)
{
    //解码图像内存申请， 来自PSRAM
    decoded_image = (uint8_t *)heap_caps_malloc(EPD_WIDTH * EPD_HEIGHT, MALLOC_CAP_SPIRAM);
    if (decoded_image == NULL)
    {
        ESP_LOGE(TAG, "Initial alloc decoded_image failed!");
    }
    memset(decoded_image, 255, EPD_WIDTH * EPD_HEIGHT);

    ESP_LOGI(TAG, "Free heap after buffers allocation: %d", xPortGetFreeHeapSize());

    double gammaCorrection = 1.0 / gamma_value;
    for (int gray_value = 0; gray_value < 256; gray_value++)
        gamme_curve[gray_value] = round(255 * pow(gray_value / 255.0, gammaCorrection));
}


// TODO
#if 0
void show_jpg_from_spiffs(const char *fn)
{
    uint8_t tmp_img_buff[1024] = { 0 };

    ESP_LOGI(TAG, "fn: %s", fn);

    File jpegFile = SPIFFS.open(fn, "r");

    uint32_t read_len = 0;
    uint32_t all_read_len = 0;

    //不断循环读取,直到没有其他内容
    while (jpegFile.available())
    {
        read_len = jpegFile.read(tmp_img_buff, 1024);
        memcpy(&jpeg_buf[all_read_len], tmp_img_buff, read_len);
        all_read_len = all_read_len + read_len;
    }
    jpegFile.close();

    epd_poweron();

    memset(decoded_image, 255, EPD_WIDTH * EPD_HEIGHT);

    ESP_LOGI(TAG, "jpegFile size=%d\n", all_read_len);

    draw_jpeg(jpeg_buf, 0, 0);

#if LIBJPEG_MEASURE
    time_update_screen = esp_timer_get_time();
#endif

    //清屏
    epd_clear();
    //显示内容
    epd_draw_grayscale_image(epd_full_screen(), fb_jpg);

    epd_poweroff();

#if LIBJPEG_MEASURE
    time_update_screen = (esp_timer_get_time() - time_update_screen) / 1000;
    ESP_LOGI(TAG, "%d ms epd_hl_update_screen\n", time_update_screen);
    ESP_LOGI(TAG, "total %d ms - total time spend\n", time_update_screen + time_decomp + time_render);
#endif
}
#endif


void show_jpg_from_buff(uint8_t *buff, uint32_t buff_size, Rect_t area)
{
    if (!buff || buff_size == 0)
    {
        ESP_LOGE(TAG, "jpeg file is NULL");
        return ;
    }

    ESP_LOGI(TAG, "jpeg size: %d Byte", buff_size);

    memset(decoded_image, 255, EPD_WIDTH * EPD_HEIGHT);

    draw_jpeg(buff, &area);

#if LIBJPEG_MEASURE
    time_update_screen = esp_timer_get_time();
#endif

    epd_clear_area(area);
    epd_draw_grayscale_image(area, decoded_image);

#if LIBJPEG_MEASURE
    time_update_screen = (esp_timer_get_time() - time_update_screen) / 1000;
    ESP_LOGI(TAG, "%d ms - screen", time_update_screen);
    ESP_LOGI(TAG, "%d ms - total time spend", time_update_screen + time_decomp + time_render);
#endif
}


void libjpeg_deinit(void)
{
    free(decoded_image);
}

/******************************************************************************/
/***        local functions                                                 ***/
/******************************************************************************/

#if JPG_DITHERING
static uint8_t find_closest_palette_color(uint8_t oldpixel)
{
    return oldpixel & 0xF0;
}
#endif

static void jpeg_render(Rect_t area)
{
#if LIBJPEG_MEASURE
    time_render = esp_timer_get_time();
#endif

#if JPG_DITHERING
    unsigned long pixel = 0;
    for (uint16_t by = 0; by < EP_HEIGHT; by++)
    {
        for (uint16_t bx = 0; bx < EP_WIDTH; bx++)
        {
            int oldpixel = decoded_image[pixel];
            int newpixel = find_closest_palette_color(oldpixel);
            int quant_error = oldpixel - newpixel;
            decoded_image[pixel] = newpixel;
            if (bx < (EP_WIDTH - 1))
                decoded_image[pixel + 1] = minimum(255, decoded_image[pixel + 1] + quant_error * 7 / 16);

            if (by < (EP_HEIGHT - 1))
            {
                if (bx > 0)
                    decoded_image[pixel + EP_WIDTH - 1] = minimum(255, decoded_image[pixel + EP_WIDTH - 1] + quant_error * 3 / 16);

                decoded_image[pixel + EP_WIDTH] = minimum(255, decoded_image[pixel + EP_WIDTH] + quant_error * 5 / 16);
                if (bx < (EP_WIDTH - 1))
                    decoded_image[pixel + EP_WIDTH + 1] = minimum(255, decoded_image[pixel + EP_WIDTH + 1] + quant_error * 1 / 16);
            }
            pixel++;
        }
    }
#endif

    for (uint32_t by = 0; by < area.height; by++)
    {
        for (uint32_t bx = 0; bx < area.width; bx++)
        {
            epd_draw_pixel_area(bx, by, decoded_image[by * area.width + bx], decoded_image, area);
        }
    }

#if LIBJPEG_MEASURE
    // calculate how long it took to draw the image
    time_render = (esp_timer_get_time() - time_render) / 1000;
    ESP_LOGI(TAG, "%d ms - rgb to bitmap", time_render);
#endif
}


static void epd_draw_pixel_area(int x, int y, uint8_t color, uint8_t *framebuffer, Rect_t area)
{
    if (x < 0 || x >= EP_WIDTH) return;
    if (y < 0 || y >= EP_HEIGHT) return;

    uint8_t *buf_ptr = &framebuffer[y * area.width / 2 + x / 2];
    if (x % 2) {
        *buf_ptr = (*buf_ptr & 0x0F) | (color & 0xF0);
    } else {
        *buf_ptr = (*buf_ptr & 0xF0) | (color >> 4);
    }
}


static uint32_t feed_buffer(JDEC *jd, uint8_t *buff, uint32_t nd)
{
    uint8_t *device = (uint8_t *)jd->device;
    uint32_t count = 0;
    while (count < nd)
    {
        if (buff != NULL)
        {
            *buff++ = device[jpeg_buf_pos];
        }
        count++;
        jpeg_buf_pos++;
    }

    return count;
}


static uint32_t tjd_output(JDEC *jd, void *bitmap, JRECT *rect)
{
    esp_task_wdt_reset();

    uint32_t w = rect->right - rect->left + 1;
    uint32_t h = rect->bottom - rect->top + 1;
    uint8_t *bitmap_ptr = (uint8_t *)bitmap;

    // printf("right: %d, left: %d, bottom: %d, top: %d\n", rect->right, rect->left, rect->bottom, rect->top);

    /**
     * @todo 8bit 16bit 32bit
     */
    for (uint32_t i = 0; i < w * h; i++)
    {
        uint8_t r = *(bitmap_ptr++);
        uint8_t g = *(bitmap_ptr++);
        uint8_t b = *(bitmap_ptr++);

        /** Calculate weighted grayscale */
        //uint32_t val = ((r * 30 + g * 59 + b * 11) / 100); // original formula
        uint32_t val = (r * 38 + g * 75 + b * 15) >> 7; // @vroland recommended formula

        int xx = rect->left + i % w;
        if (xx < 0 || xx >= jd->width) continue ;

        int yy = rect->top + i / w;
        if (yy < 0 || yy >= jd->height) continue ;

        /**
         * Optimization note: If we manage to apply here the epd_draw_pixel
         * directly then it will be no need to keep a huge raw buffer (But will
         * loose dither)
         */
        decoded_image[yy * jd->width + xx] = gamme_curve[val];
    }

    return 1;
}


static int draw_jpeg(uint8_t *jpeg_buf, Rect_t *area)
{
    JDEC jd;
    JRESULT rc;

    jpeg_buf_pos = 0; //此值不要忘了初始化

    rc = jd_prepare(&jd, feed_buffer, tjpgd_work, sizeof(tjpgd_work), jpeg_buf);
    if (rc != JDR_OK)
    {
        ESP_LOGE(TAG, "prepare error: %s", jd_errors[rc]);
        return ESP_FAIL;
    }

    area->width = jd.width;
    area->height = jd.height;

#if LIBJPEG_MEASURE
    uint32_t decode_start = esp_timer_get_time();
#endif

    rc = jd_decomp(&jd, tjd_output, 0);
    if (rc != JDR_OK)
    {
        ESP_LOGE(TAG, "decomp error: %s", jd_errors[rc]);
        return ESP_FAIL;
    }

#if LIBJPEG_MEASURE
    time_decomp = (esp_timer_get_time() - decode_start) / 1000;
    ESP_LOGI(TAG, "jpeg file width: %d, height: %d", jd.width, jd.height);
    ESP_LOGI(TAG, "%d ms - image decompression", time_decomp);
#endif

    // Render the image onto the screen at given coordinates
    jpeg_render(*area);

    return 1;
}


/******************************************************************************/
/***        END OF FILE                                                     ***/
/******************************************************************************/