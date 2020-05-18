/*
 * att_est_q.c
 *
 *  Created on: Dec 13, 2019
 *      Author: kychu
 */

#include "att_est_q.h"
#include "algorithm.h"

#include "log.h"
#include "mesg.h"
#include "kyLink.h"
#include "compass.h"

#include "cpu_utils.h"

#include <string.h>

static const char *TAG = "SINS";

static Euler_T est_e = {0, 0, 0};
static Quat_T est_q = {1, 0, 0, 0};

static IMU_RAW_6DOF imu_raw;
static IMU_UNIT_6DOF imu_unit_6dof;
static IMU_UNIT_9DOF imu_unit_9dof;

//static _3AxisUnit gyr_off;
static uint32_t imu_selftest_done = 0;
static uint32_t gyr_peace_flag = 0;

static uint32_t msg_quat_ts = 0;
static struct MsgInfo msg_quat = {
  TYPE_QUAT_Info_Resp,
  1,
  100,
  {0, 0, 0, 0, 0}
};

static struct MsgList msg_list_quat = {
  &msg_quat,
  NULL
};

static void att_est_q_task(void const *argument);
static void imu_peace_check(_3AxisUnit *pgyr);

static void att_est_q_task(void const *argument)
{
  (void) argument;
  uint8_t st_ret = 0;
  uint32_t test_ts = 0;
  float delta_t = 1e-3;
  uint32_t last_t_us = 0, delta_t_us = 0;

  Vector3D mag_data;

  uint32_t time_now;

  ky_info(TAG, "att est task start.");
  osDelay(100);
  if(icm42605_init() != status_ok) {
	  ky_err(TAG, "imu sensor init failed. EXIT!");
	  vTaskDelete(NULL);
	  return;
  }
  ky_info(TAG, "keep IMU motionless ...");
  while(imu_selftest_done == 0) {
    /* read IMU data form sensor */
    if(icm42605_read(&imu_raw, &imu_unit_6dof, 1000) == status_ok) {
      /* check for motionless state */
      imu_peace_check(&imu_unit_6dof.Gyr);
      if(gyr_peace_flag == 1) {
        if(icm42605_selftest(&imu_raw, &st_ret) != status_ok) {
          ky_err(TAG, "imu self test faileds. EXIT!");
          vTaskDelete(NULL);
          return;
        }

        if(st_ret & 0x01)
          ky_info(TAG, "gyr st PASS.");
        else
          ky_info(TAG, "gyr st FAIL.");

        if(st_ret & 0x02)
          ky_info(TAG, "acc st PASS.");
        else
          ky_info(TAG, "acc st FAIL.");
        imu_selftest_done = 1;
      } else {
        time_now = xTaskGetTickCountFromISR();
        if((time_now - test_ts) >= 1000) {
          test_ts = time_now;
          ky_warn(TAG, "waiting. %d", imu_raw.Gyr.X);
        }
      }
    } else {
      ky_err(TAG, "read sensor timeout!");
    }
  }

  mesg_publish_mesg(&msg_list_quat);

  for(;;) {
    /* read IMU data form sensor */
    if(icm42605_read(&imu_raw, &imu_unit_6dof, osWaitForever) == status_ok) {
      /* compute delta time according to time-stamp */
      if(imu_raw.TS > last_t_us)
        delta_t_us = imu_raw.TS - last_t_us;
      else
        delta_t_us = 0x10000 - last_t_us + imu_raw.TS;
      last_t_us = imu_raw.TS;
      delta_t = delta_t_us * 1e-6;

//      imu_peace_check(&imu_unit.Gyr);

      if(magnetics_take(&mag_data) == 0) {
        imu_unit_9dof.Acc = imu_unit_6dof.Acc;
        imu_unit_9dof.Gyr = imu_unit_6dof.Gyr;
        imu_unit_9dof.Temp = imu_unit_6dof.Temp;
        imu_unit_9dof.TS = imu_unit_6dof.TS;
        imu_unit_9dof.Mag = *(_3AxisUnit *)&mag_data;
        fusionQ_9dot(&imu_unit_9dof, &est_q, 5, 0, delta_t);
      } else
        fusionQ_6dot(&imu_unit_6dof, &est_q, 5, 0, delta_t);
      Quat2Euler(&est_q, &est_e);

      /* update message */
      time_now = xTaskGetTickCountFromISR();

      if((msg_quat.msg_st & 0x01) && (msg_quat.msg_rt != 0)) {
        if((time_now - msg_quat_ts) >= 1000 / msg_quat.msg_rt) {
          msg_quat_ts = time_now;
          mesg_send_mesg((const void *)&est_q, TYPE_QUAT_Info_Resp, sizeof(Quat_T));
        }
      }
    }
  }
}

void att_est_get_euler(Euler_T *e)
{
  *e = est_e;
}

#define GYR_PEACE_THRESHOLD                      (0.5f) /* unit: dps */

static _3AxisUnit last_gyr = {0};
static void imu_peace_check(_3AxisUnit *pgyr)
{
  static uint32_t peace_ts = 0;
  static uint32_t peace_tn = 0;

  peace_tn = xTaskGetTickCount();
  if((fabs(pgyr->X - last_gyr.X) < GYR_PEACE_THRESHOLD) &&
     (fabs(pgyr->Y - last_gyr.Y) < GYR_PEACE_THRESHOLD) &&
     (fabs(pgyr->Z - last_gyr.Z) < GYR_PEACE_THRESHOLD)) {
    if(peace_tn - peace_ts > configTICK_RATE_HZ) { // keep 1s.
      if(gyr_peace_flag == 0) {
        gyr_peace_flag = 1;
//        icm42605_gyr_offset(pgyr);
      }
    }
  } else {
    gyr_peace_flag = 0;
    peace_ts = peace_tn;
  }

  /* update the last data */
  last_gyr.X = pgyr->X;
  last_gyr.Y = pgyr->Y;
  last_gyr.Z = pgyr->Z;
}

static osThreadId sins_task_id = NULL;
osThreadDef(SINS, att_est_q_task, osPriorityNormal, 0, 512);

int sins_main(int argc, char **argv)
{
  if(argc < 2) return -1;
  if(strcmp(argv[1], "start") == 0) {
    if(sins_task_id == NULL) {
      sins_task_id = osThreadCreate(osThread(SINS), NULL);
      if(sins_task_id == NULL) {
        ky_err(TAG, "task create failed");
        return -2;
      }
    } else {
      ky_warn(TAG, "already started");
    }
    return 0;
  }
  if(strcmp(argv[1], "stop") == 0) {
    if(sins_task_id != NULL) {
      vTaskDelete(sins_task_id);
      sins_task_id = NULL;
      return 0;
    }
  }
  return -1;
}

/******************** kyChu<kyChu@qq.com> **** END OF FILE ********************/
