/**
  ******************************************************************************
  * @file    ./pid_ctrl.c
  * @author  kyChu
  * @version V1.0.0
  * @date    30-April-2020
  * @brief   PID controller function.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "algorithm.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*
 * PID controller algorithm
 */
void pid_loop(PID* pid, float expect, float measure)
{
  float err, diff;
  err = expect - measure;

  if(isnan(err)) return;

  diff = (err - pid->preErr) / pid->dt;
  pid->preErr = err;

  pid->I_sum += err * pid->dt;
  if(pid->I_sum > pid->I_max )
    pid->I_sum = pid->I_max;
  if(pid->I_sum < -pid->I_max )
    pid->I_sum = -pid->I_max;

  diff = pid->kd * diff;
  if(diff > pid->D_max) diff = pid->D_max;
  if(diff < -pid->D_max) diff = -pid->D_max;

  pid->Output = pid->kp * err + diff + pid->ki * pid->I_sum;
}
