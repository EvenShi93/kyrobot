/*
 * remote_task.c
 *
 *  Created on: Apr 27, 2020
 *      Author: kychu
 */

#include "apps.h"

#include "rf_type.h"
#include "rf_evt_proc.h"

static const char *TAG = "RF";

#define RMT_PROC_TASK_TEST_ENABLE             (0)

#define RMT_DECODER_CACHE_SIZE                (20)
#define RMT_RECV_CACHE_SIZE                   (24)

static osThreadId rmt_proc_id = NULL;

static uint32_t _task_should_exit = 0;

static void rmt_proc_task(void const *argument);
static void rmt_decode_callback(kyLinkBlockDef *pBlock);

static struct rf_info_t {
  uint32_t rf_timestamp;
  uint32_t rf_lost_flag;
  rf_ctrl_t rf_data;
} *rf_info = NULL;

extern const uart_dev_t usart2_dev;

static void rmt_proc_task(void const *argument)
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

  ky_info(TAG, "rf task started");

  if(rmt_uart->uart_init(115200) != status_ok) {
    ky_err(TAG, "failed to init rmt_uart");
    rmt_proc_id = NULL;
    vTaskDelete(NULL);
  }

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
  while(!_task_should_exit) {
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
  rmt_proc_id = NULL;
  ky_warn(TAG, "exit");
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

osThreadDef(REMT, rmt_proc_task, osPriorityNormal, 0, 256);

int remote_main(int argc, char **argv)
{
  if(argc < 2) return -1;
  if(strcmp(argv[1], "start") == 0) {
    if(rmt_proc_id == NULL) {
      _task_should_exit = 0;
      rmt_proc_id = osThreadCreate(osThread(REMT), NULL);
      if(rmt_proc_id == NULL) {
        ky_err(TAG, "task create failed.");
        return -2;
      }
    } else {
      ky_warn(TAG, "already started");
    }
    return 0;
  }
  if(strcmp(argv[1], "stop") == 0) {
    if(rmt_proc_id != NULL) {
      _task_should_exit = 1;
      return 0;
    }
  }
  return -1;
}
