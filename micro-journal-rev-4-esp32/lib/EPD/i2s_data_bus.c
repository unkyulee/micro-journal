
/******************************************************************************/
/***        include files                                                   ***/
/******************************************************************************/

#include "i2s_data_bus.h"

#include <esp_heap_caps.h>
#include "esp_lcd_panel_io.h"
#include "esp_err.h"
#include "esp_log.h"

/******************************************************************************/
/***        macro definitions                                               ***/
/******************************************************************************/

/*
 * NOTE: despite the historical filename, on this project the pixel bus is
 * driven through ESP-IDF's generic Intel 8080 (i80) LCD driver, not by
 * hand-programmed I2S registers. The original register-level I2S
 * implementation (selected by a USER_I2S_REG macro) was dead code on every
 * supported target and has been removed; see git history if it is ever
 * needed again.
 */

static const char *TAG = "I80";

/******************************************************************************/
/***        local variables                                                 ***/
/******************************************************************************/

static esp_lcd_panel_io_handle_t io_handle = NULL;

/**
 * @brief Indicates the device has finished its transmission and is ready again.
 */
static volatile bool output_done = true;

/**
 * @brief Line transfer buffer. Sized for the ED047TC1's 960px row plus the
 *        32px dummy tail the original driver always transmitted for timing
 *        headroom (4 pixels per byte on the wire).
 */
static uint8_t buffer[(960 + 32) / 4] = { 0 };

/******************************************************************************/
/***        exported functions                                              ***/
/******************************************************************************/

volatile uint8_t IRAM_ATTR *i2s_get_current_buffer()
{
    return buffer;
}


bool IRAM_ATTR i2s_is_busy()
{
    return !output_done;
}


void IRAM_ATTR i2s_switch_buffer()
{
}


void IRAM_ATTR i2s_start_line_output()
{
    output_done = false;

    esp_lcd_panel_io_tx_color(io_handle, 0, buffer, (960 + 32) / 4);
}


static bool notify_trans_done(esp_lcd_panel_io_handle_t panel_io, esp_lcd_panel_io_event_data_t *edata, void *user_ctx)
{
    output_done = true;
    return output_done;
}


void i2s_bus_init(i2s_bus_config *cfg)
{
    ESP_LOGI(TAG, "Initialize Intel 8080 bus");
    esp_lcd_i80_bus_handle_t i80_bus = NULL;
    esp_lcd_i80_bus_config_t bus_config = {
        .dc_gpio_num = cfg->start_pulse,
        .wr_gpio_num = cfg->clock,
        .data_gpio_nums = {
            cfg->data_6,
            cfg->data_7,
            cfg->data_4,
            cfg->data_5,
            cfg->data_2,
            cfg->data_3,
            cfg->data_0,
            cfg->data_1,
        },
        .bus_width = 8,
        .max_transfer_bytes = (cfg->epd_row_width + 32)/4
    };
    ESP_ERROR_CHECK(esp_lcd_new_i80_bus(&bus_config, &i80_bus));

    esp_lcd_panel_io_i80_config_t io_config = {
        .cs_gpio_num = -1,
        .pclk_hz = 10 * 1000 * 1000,
        .trans_queue_depth = 10,
        .dc_levels = {
            .dc_idle_level = 0,
            .dc_cmd_level = 1,
            .dc_dummy_level = 0,
            .dc_data_level = 0,
        },
        .on_color_trans_done = notify_trans_done,
        .user_ctx = NULL,
        .lcd_cmd_bits = 10,
        .lcd_param_bits = 0,
        // .flags.reverse_color_bits = 1
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_i80(i80_bus, &io_config, &io_handle));
}

/******************************************************************************/
/***        END OF FILE                                                     ***/
/******************************************************************************/
