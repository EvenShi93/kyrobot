/*
 * ctrl_task.c
 *
 *  Created on: Apr 22, 2020
 *      Author: kychu
 */

#include "apps.h"

#include "ctrl_task.h"

static const char *TAG = "CTRL";

void ctrl_task(void const *argument)
{
  ky_info(TAG, "ctrl task started.");
  if(motorif_init() != status_ok) {
    ky_err(TAG, "MOTOR initialize failed");
    vTaskDelete(NULL);
  }
  if(steerif_init(5000, 1500) != status_ok) {
    ky_err(TAG, "Failed to initialize STEER");
    vTaskDelete(NULL);
  }
  ky_info(TAG, "steer driver initialized");
  for(;;) {
    delay(1000);
  }
}
