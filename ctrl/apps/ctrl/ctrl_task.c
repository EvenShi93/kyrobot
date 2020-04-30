/*
 * ctrl_task.c
 *
 *  Created on: Apr 22, 2020
 *      Author: kychu
 */

#include "apps.h"

#include "algorithm.h"

#include "ctrl_task.h"
#include "rf_evt_proc.h"

static const char *TAG = "CTRL";

#define MOTOR_ENCODER_COUNTER_TEST     (0)

#define STEER_CTRL_PWM_POS_MIN         900
#define STEER_CTRL_PWM_POS_MID         1400
#define STEER_CTRL_PWM_POS_MAX         1900

#define STEER_CTRL_PWM_PERIOD          5000

#define MOTOR_CTRL_PWM_POS_MIN         -1000
#define MOTOR_CTRL_PWM_POS_MID         0
#define MOTOR_CTRL_PWM_POS_MAX         1000

#define MOTOR_VEL_EXP_MIN              -2500
#define MOTOR_VEL_EXP_MID              0
#define MOTOR_VEL_EXP_MAX              2500

#define CONTROL_LOOP_PERIOD            (10)

static float motor_exp;
static int16_t motor_vel;

/*
 * used to keep ctrl_task task handle
 */
static osThreadId ctrlThread;

static void ctrl_task_notify(void);

void ctrl_task(void const *argument)
{
  PID *ctrl_pid;
  float steer_exp, motor_ctr = 0;
#if MOTOR_ENCODER_COUNTER_TEST
  uint32_t task_timestamp = 0, log_ts = 0;
#endif /* MOTOR_ENCODER_COUNTER_TEST */
  rf_ctrl_t *rf_data;
  if(motorif_init() != status_ok) {
    ky_err(TAG, "MOTOR initialize failed");
    goto exit;
  }
  if(encoderif_init() != status_ok) {
    ky_err(TAG, "Encoder INIT FAIL");
    goto exit;
  }
  if(steerif_init(STEER_CTRL_PWM_PERIOD, STEER_CTRL_PWM_POS_MID) != status_ok) {
    ky_err(TAG, "Failed to initialize STEER");
    goto exit;
  }
  ctrlThread = xTaskGetCurrentTaskHandle();
  if(notify_timer_init(CONTROL_LOOP_PERIOD, ctrl_task_notify) != status_ok) {
    ky_err(TAG, "notify timer start failed");
    goto exit;
  }
  rf_data = kmm_alloc(sizeof(rf_ctrl_t));
  ctrl_pid = kmm_alloc(sizeof(PID));
  if(rf_data == NULL || ctrl_pid == NULL) {
    kmm_free(rf_data);
    kmm_free(ctrl_pid);
    ky_err(TAG, "no memory for CTRL");
    vTaskDelete(NULL);
  }
  ctrl_pid->kp = 0.15f;
  ctrl_pid->ki = 0.00f;
  ctrl_pid->kd = 0.01f;
  ctrl_pid->preErr = 0.0f;
  ctrl_pid->D_max = 500.0f;
  ctrl_pid->I_max = 500.0f;
  ctrl_pid->I_sum = 0.0f;
  ctrl_pid->Output = 0.0f;
  ctrl_pid->dt = CONTROL_LOOP_PERIOD * 0.001f;
  ky_info(TAG, "ctrl task started.");
  for(;;) {
    if(xTaskNotifyWait(0xFFFFFFFF, 0xFFFFFFFF, NULL, 100) == pdTRUE) {
      rmt_get_channels(rf_data);
      // steer control
      steer_exp = (rf_data->Roll - RF_CHANNEL_VAL_MID) * (STEER_CTRL_PWM_POS_MAX - STEER_CTRL_PWM_POS_MIN);
      steer_exp /= (RF_CHANNEL_VAL_MAX - RF_CHANNEL_VAL_MIN);
      steer_exp += STEER_CTRL_PWM_POS_MID;
      steerif_set_poswidth((uint32_t)steer_exp);
      // motor control
      motor_exp = apply_deadband(rf_data->Pitch - RF_CHANNEL_VAL_MID, 80);
      motor_exp = motor_exp * (MOTOR_VEL_EXP_MAX - MOTOR_VEL_EXP_MIN);
      motor_exp /= (RF_CHANNEL_VAL_MAX - RF_CHANNEL_VAL_MIN);
      motor_exp += MOTOR_VEL_EXP_MID;

      motor_vel = encoderif_take_counter();
#if MOTOR_ENCODER_COUNTER_TEST
      task_timestamp = xTaskGetTickCountFromISR();
      if((task_timestamp - log_ts) > 1000) {
        log_ts = task_timestamp;
        ky_info(TAG, "encoder val: %d", motor_vel);
      }
#endif /* MOTOR_ENCODER_COUNTER_TEST */
      pid_loop(ctrl_pid, motor_exp, motor_vel);
      motor_ctr += ctrl_pid->Output;
      // if stopped, close motor output.
      if((motor_exp == MOTOR_VEL_EXP_MID) && (motor_vel == MOTOR_VEL_EXP_MID)) {
        motor_ctr = 0;
        ctrl_pid->I_sum = 0;
        ctrl_pid->preErr = 0;
      }
      if(motor_ctr > 1000) motor_ctr = 1000;
      if(motor_ctr < -1000) motor_ctr = -1000;
      if(motor_ctr > 0) {
        motorif_set_dutycycle_a(0);
        motorif_set_dutycycle_b(motor_ctr);
      } else {
        motorif_set_dutycycle_a(-motor_ctr);
        motorif_set_dutycycle_b(0);
      }
    }
  }
exit:
  vTaskDelete(NULL);
}

static void ctrl_task_notify(void)
{
  osSignalSet(ctrlThread, 0x00000001);
}

float ctrl_get_motor_exp(void) { return motor_exp; }
float ctrl_get_motor_vel(void) { return motor_vel; }
