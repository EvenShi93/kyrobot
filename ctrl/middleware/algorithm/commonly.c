/**
  ******************************************************************************
  * @file    ./commonly.c
  * @author  kyChu
  * @version V1.0.0
  * @date    8-January-2020
  * @brief   commonly used functions.
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

void NormalizeVector(Vector3D *v)
{
  float recipNorm = 1.0f / sqrtf(v->X * v->X + v->Y * v->Y + v->Z * v->Z);
  v->X *= recipNorm;
  v->Y *= recipNorm;
  v->Z *= recipNorm;
}

float ScalarProduct(Vector3D *va, Vector3D *vb)
{
  return (va->X * vb->X + va->Y * vb->Y + va->Z * vb->Z);
}

/*
 * apply deadband function.
 */
float apply_deadband(float value, float deadband)
{
	if(fabs(value) <= deadband) {value = 0;}

	if(value > deadband) {value -= deadband;}
	if(value < -deadband) {value += deadband;}

	return value;
}

/*
 * step change function.
 */
void step_change(float *in, float target, float step, float deadBand)
{
	if(fabsf(*in - target) <= deadBand) {*in = target; return;}

	if(*in > target) {*in -= fabsf(step); return;}
	if(*in < target) {*in += fabsf(step); return;}
}

/*
 * limitation
 */
float apply_limit(float in, float min, float max)
{
  if(in < min) return min;
  if(in > max) return max;
  return in;
}
