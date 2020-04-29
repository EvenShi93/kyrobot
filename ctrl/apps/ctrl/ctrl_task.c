/*
 * ctrl_task.c
 *
 *  Created on: Apr 22, 2020
 *      Author: kychu
 */

#include "apps.h"

#include "ctrl_task.h"
#include "rf_evt_proc.h"

static const char *TAG = "CTRL";

#define STEER_CTRL_PWM_POS_MIN         900
#define STEER_CTRL_PWM_POS_MID         1400
#define STEER_CTRL_PWM_POS_MAX         1900

#define STEER_CTRL_PWM_PERIOD          5000

#define MOTOR_CTRL_PWM_POS_MIN         -1000
#define MOTOR_CTRL_PWM_POS_MID         0
#define MOTOR_CTRL_PWM_POS_MAX         1000

void ctrl_task(void const *argument)
{
  float rf_temp;
  rf_ctrl_t *rf_data;
  if(motorif_init() != status_ok) {
    ky_err(TAG, "MOTOR initialize failed");
    vTaskDelete(NULL);
  }
  if(steerif_init(STEER_CTRL_PWM_PERIOD, STEER_CTRL_PWM_POS_MID) != status_ok) {
    ky_err(TAG, "Failed to initialize STEER");
    vTaskDelete(NULL);
  }
  rf_data = kmm_alloc(sizeof(rf_ctrl_t));
  if(rf_data == NULL) {
    ky_err(TAG, "no memory for CTRL");
    vTaskDelete(NULL);
  }
  ky_info(TAG, "ctrl task started.");
  for(;;) {
    delay(10);
    rmt_get_channels(rf_data);
    rf_temp = (rf_data->Roll - RF_CHANNEL_VAL_MID) * (STEER_CTRL_PWM_POS_MAX - STEER_CTRL_PWM_POS_MIN);
    rf_temp /= (RF_CHANNEL_VAL_MAX - RF_CHANNEL_VAL_MIN);
    rf_temp += STEER_CTRL_PWM_POS_MID;
    steerif_set_poswidth((uint32_t)rf_temp);
    rf_temp = (rf_data->Pitch - RF_CHANNEL_VAL_MID) * (MOTOR_CTRL_PWM_POS_MAX - MOTOR_CTRL_PWM_POS_MIN);
    rf_temp /= (RF_CHANNEL_VAL_MAX - RF_CHANNEL_VAL_MIN);
    rf_temp += MOTOR_CTRL_PWM_POS_MID;
    if(rf_temp > 0) {
      motorif_set_dutycycle_a(0);
      motorif_set_dutycycle_b(rf_temp);
    } else {
      motorif_set_dutycycle_a(-rf_temp);
      motorif_set_dutycycle_b(0);
    }
  }
}
