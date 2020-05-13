/*
 * remote_task.c
 *
 *  Created on: Apr 27, 2020
 *      Author: kychu
 */

#include "apps.h"

#include "rf_type.h"
#include "rf_evt_proc.h"

const static char *TAG = "RMTP";

#define RMT_PROC_TASK_TEST_ENABLE             (0)

#define RMT_DECODER_CACHE_SIZE                (20)
#define RMT_RECV_CACHE_SIZE                   (24)

static void rmt_decode_callback(kyLinkBlockDef *pBlock);

static struct rf_info_t {
  uint32_t rf_timestamp;
  uint32_t rf_lost_flag;
  rf_ctrl_t rf_data;
} *rf_info;

extern const uart_dev_t usart2_dev;

void rmt_proc_task(void const *argument)
{
  const uart_dev_t *rmt_uart = &usart2_dev;
  kyLinkConfig_t *cfg = NULL;
  KYLINK_CORE_HANDLE *kylink_rmt;
  uint8_t *kylink_decoder_cache;
  uint8_t *recv_cache;
  uint32_t recv_len, decode_cnt;
#if RMT_PROC_TASK_TEST_ENABLE
  uint32_t task_timestamp = 0, log_ts = 0;
#endif /* RMT_PROC_TASK_TEST_ENABLE */

  cfg = kmm_alloc(sizeof(kyLinkConfig_t));
  recv_cache = kmm_alloc(RMT_RECV_CACHE_SIZE);
  rf_info = kmm_alloc(sizeof(struct rf_info_t));
  kylink_rmt = kmm_alloc(sizeof(KYLINK_CORE_HANDLE));
  kylink_decoder_cache = kmm_alloc(RMT_DECODER_CACHE_SIZE);
  if(rf_info == NULL || recv_cache == NULL || cfg == NULL || kylink_rmt == NULL || kylink_decoder_cache == NULL) {
    goto exit;
    ky_err(TAG, "RMT memory alloc failed, EXIT!");
  }

  rf_info->rf_lost_flag = 1; // default state: signal lost
  // reset all channels
  rf_info->rf_data.Pitch = RF_CHANNEL_VAL_MID;
  rf_info->rf_data.Roll = RF_CHANNEL_VAL_MID;
  rf_info->rf_data.Yaw = RF_CHANNEL_VAL_MID;
  rf_info->rf_data.Throttle = RF_CHANNEL_VAL_MIN;
  rf_info->rf_timestamp = 0;
  memset(recv_cache, 0, RMT_RECV_CACHE_SIZE);
  cfg->cache_size = RMT_DECODER_CACHE_SIZE;
  cfg->decoder_cache = kylink_decoder_cache;
  cfg->callback = rmt_decode_callback;
  cfg->txfunc = NULL;
  kylink_init(kylink_rmt, cfg);
  kmm_free(cfg);

  for(;;) {
    delay(25);
    do {
      recv_len = rmt_uart->uart_rx(recv_cache, RMT_RECV_CACHE_SIZE);
      if(recv_len > 0) {
        decode_cnt = 0;
        while(decode_cnt < recv_len) {
          kylink_decode(kylink_rmt, recv_cache[decode_cnt ++]);
        }
      }
    } while(recv_len > 0);
    if((xTaskGetTickCountFromISR() - rf_info->rf_timestamp) > 1000) {
      if(rf_info->rf_lost_flag == 0) {
        rf_info->rf_lost_flag = 1;
        // reset all channels
        rf_info->rf_data.Pitch = RF_CHANNEL_VAL_MID;
        rf_info->rf_data.Roll = RF_CHANNEL_VAL_MID;
        rf_info->rf_data.Yaw = RF_CHANNEL_VAL_MID;
        rf_info->rf_data.Throttle = RF_CHANNEL_VAL_MIN;
        ky_warn(TAG, "rf signal lost");
      }
    }
#if RMT_PROC_TASK_TEST_ENABLE
    task_timestamp = xTaskGetTickCountFromISR();
    if((task_timestamp - log_ts) > 1000) {
      log_ts = task_timestamp;
      ky_info(TAG, "thr: %d, pit: %d, rol: %d, yaw: %d, mod: %d",
        rf_info->rf_data.Throttle, rf_info->rf_data.Pitch, rf_info->rf_data.Roll, rf_info->rf_data.Yaw, rf_info->rf_data.mode);
    }
#endif /* RMT_PROC_TASK_TEST_ENABLE */
  }

exit:
  kmm_free(cfg);
  kmm_free(rf_info);
  kmm_free(kylink_rmt);
  kmm_free(recv_cache);
  kmm_free(kylink_decoder_cache);
  if(rmt_uart != NULL) {
    rmt_uart->uart_deinit();
    rmt_uart = NULL;
  }
  vTaskDelete(NULL);
}

uint16_t rmt_get_thr_chan(void) { return rf_info->rf_data.Throttle; }
uint16_t rmt_get_yaw_chan(void) { return rf_info->rf_data.Yaw; }
uint16_t rmt_get_pit_chan(void) { return rf_info->rf_data.Pitch; }
uint16_t rmt_get_rol_chan(void) { return rf_info->rf_data.Roll; }

uint32_t rmt_is_signallost(void) { return rf_info->rf_lost_flag; }

void rmt_get_channels(rf_ctrl_t *rf) { *rf = rf_info->rf_data; }

static void rmt_decode_callback(kyLinkBlockDef *pBlock)
{
  if(pBlock->msg_id == 0x22 && pBlock->dev_id == 0x11) {
    rf_info->rf_data = *(rf_ctrl_t *)pBlock->buffer;
    rf_info->rf_timestamp = xTaskGetTickCountFromISR();
    rf_info->rf_lost_flag = 0;
  }
}
