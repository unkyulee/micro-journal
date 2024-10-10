
/******************************************************************************/
/***        include files                                                   ***/
/******************************************************************************/

#include "rmt_pulse.h"

#include <driver/rmt.h>

/******************************************************************************/
/***        macro definitions                                               ***/
/******************************************************************************/

/******************************************************************************/
/***        type definitions                                                ***/
/******************************************************************************/

/******************************************************************************/
/***        local function prototypes                                       ***/
/******************************************************************************/

/**
 * @brief Remote peripheral interrupt. Used to signal when transmission is done.
 */
static void IRAM_ATTR rmt_interrupt_handler(void *arg);

/******************************************************************************/
/***        exported variables                                              ***/
/******************************************************************************/

/******************************************************************************/
/***        local variables                                                 ***/
/******************************************************************************/

static intr_handle_t gRMT_intr_handle = NULL;

/**
 * @brief the RMT channel configuration object
 */
static rmt_config_t row_rmt_config;

/**
 * @brief keep track of wether the current pulse is ongoing
 */
// static volatile bool rmt_tx_done = true;

/******************************************************************************/
/***        exported functions                                              ***/
/******************************************************************************/

void rmt_pulse_init(gpio_num_t pin)
{
    row_rmt_config.rmt_mode = RMT_MODE_TX;
    // currently hardcoded: use channel 0
    row_rmt_config.channel = RMT_CHANNEL_1;

    row_rmt_config.gpio_num = pin;
    row_rmt_config.mem_block_num = 2;

    // Divide 80MHz APB Clock by 8 -> .1us resolution delay
    row_rmt_config.clk_div = 8;

    row_rmt_config.tx_config.loop_en = false;
    row_rmt_config.tx_config.carrier_en = false;
    row_rmt_config.tx_config.carrier_level = RMT_CARRIER_LEVEL_LOW;
    row_rmt_config.tx_config.idle_level = RMT_IDLE_LEVEL_LOW;
    row_rmt_config.tx_config.idle_output_en = true;

// #if ESP_IDF_VERSION_MAJOR >= 4
//     rmt_isr_register(rmt_interrupt_handler, 0,
//                      ESP_INTR_FLAG_LEVEL3, &gRMT_intr_handle);
// #else
//     esp_intr_alloc(ETS_RMT_INTR_SOURCE, ESP_INTR_FLAG_LEVEL3,
//                    rmt_interrupt_handler, 0, &gRMT_intr_handle);
// #endif

    rmt_config(&row_rmt_config);
    rmt_driver_install(RMT_CHANNEL_1, 0, 0);
    // rmt_set_tx_intr_en(row_rmt_config.channel, true);
}


void IRAM_ATTR pulse_ckv_ticks(uint16_t high_time_ticks,
                               uint16_t low_time_ticks, bool wait)
{
    // while (!rmt_tx_done) ;

    rmt_item32_t rmt_mem_ptr;
    if (high_time_ticks > 0)
    {
        rmt_mem_ptr.level0 = 1;
        rmt_mem_ptr.duration0 = high_time_ticks;
        rmt_mem_ptr.level1 = 0;
        rmt_mem_ptr.duration1 = low_time_ticks;
    }
    else
    {
        rmt_mem_ptr.level0 = 1;
        rmt_mem_ptr.duration0 = low_time_ticks;
        rmt_mem_ptr.level1 = 0;
        rmt_mem_ptr.duration1 = 0;
    }
    // RMTMEM.chan[row_rmt_config.channel].data32[1].val = 0;
    // rmt_tx_done = false;
    // RMT.conf_ch[row_rmt_config.channel].conf1.mem_rd_rst = 1;
    // RMT.conf_ch[row_rmt_config.channel].conf1.mem_owner = RMT_MEM_OWNER_TX;
    // RMT.conf_ch[row_rmt_config.channel].conf1.tx_start = 1;
    rmt_write_items(row_rmt_config.channel, &rmt_mem_ptr, 1, wait);
    // while (wait && !rmt_tx_done) ;
}


void IRAM_ATTR pulse_ckv_us(uint16_t high_time_us, uint16_t low_time_us, bool wait)
{
    pulse_ckv_ticks(10 * high_time_us, 10 * low_time_us, wait);
}


// bool IRAM_ATTR rmt_busy()
// {
//     return !rmt_tx_done;
// }

/******************************************************************************/
/***        local functions                                                 ***/
/******************************************************************************/

// static void IRAM_ATTR rmt_interrupt_handler(void *arg)
// {
//     rmt_tx_done = true;
//     RMT.int_clr.val = RMT.int_st.val;
// }

/******************************************************************************/
/***        END OF FILE                                                     ***/
/******************************************************************************/