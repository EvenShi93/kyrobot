/*
 * remote_task.c
 *
 *  Created on: Apr 27, 2020
 *      Author: kychu
 */

#include "apps.h"

#include "rf_type.h"

const static char *TAG = "RMTP";

#define RMT_PROC_TASK_TEST_ENABLE             (0)

#define RMT_DECODER_CACHE_SIZE                (20)
#define RMT_RECV_CACHE_SIZE                   (24)

static void rmt_decode_callback(kyLinkBlockDef *pBlock);

static rf_ctrl_t *rf_data;

void rmt_proc_task(void const *argument)
{
  kyLinkConfig_t *cfg = NULL;
  KYLINK_CORE_HANDLE *kylink_rmt;
  uint8_t *kylink_decoder_cache;
  uint8_t *recv_cache;
  uint32_t recv_len, decode_cnt;
#if RMT_PROC_TASK_TEST_ENABLE
  uint32_t task_timestamp = 0, log_ts = 0;
#endif /* RMT_PROC_TASK_TEST_ENABLE */

  if(rmtif_init() != status_ok) {
    ky_err(TAG, "RF IF INIT FAILED");
    vTaskDelete(NULL);
  }
  rf_data = kmm_alloc(sizeof(rf_ctrl_t));
  cfg = kmm_alloc(sizeof(kyLinkConfig_t));
  recv_cache = kmm_alloc(RMT_RECV_CACHE_SIZE);
  kylink_rmt = kmm_alloc(sizeof(KYLINK_CORE_HANDLE));
  kylink_decoder_cache = kmm_alloc(RMT_DECODER_CACHE_SIZE);
  if(rf_data == NULL || recv_cache == NULL || cfg == NULL || kylink_rmt == NULL || kylink_decoder_cache == NULL) {
    goto exit;
    ky_err(TAG, "RMT memory alloc failed, EXIT!");
  }

  memset(rf_data, 0, sizeof(rf_ctrl_t));
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
      recv_len = rmtif_rx_bytes(recv_cache, RMT_RECV_CACHE_SIZE);
      if(recv_len > 0) {
        decode_cnt = 0;
        while(decode_cnt < recv_len) {
          kylink_decode(kylink_rmt, recv_cache[decode_cnt ++]);
        }
      }
    } while(recv_len > 0);
#if RMT_PROC_TASK_TEST_ENABLE
    task_timestamp = xTaskGetTickCountFromISR();
    if((task_timestamp - log_ts) > 1000) {
      log_ts = task_timestamp;
      ky_info(TAG, "thr: %d, pit: %d, rol: %d, yaw: %d, mod: %d", rf_data->Throttle, rf_data->Pitch, rf_data->Roll, rf_data->Yaw, rf_data->mode);
    }
#endif /* RMT_PROC_TASK_TEST_ENABLE */
  }

exit:
  kmm_free(cfg);
  kmm_free(rf_data);
  kmm_free(kylink_rmt);
  kmm_free(recv_cache);
  kmm_free(kylink_decoder_cache);
  vTaskDelete(NULL);
}

static void rmt_decode_callback(kyLinkBlockDef *pBlock)
{
  if(pBlock->msg_id == 0x22 && pBlock->dev_id == 0x11) {
    *rf_data = *(rf_ctrl_t *)pBlock->buffer;
  }
}
