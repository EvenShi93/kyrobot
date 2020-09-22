/**
  ******************************************************************************
  * @file    ./quat_est.c
  * @author  kyChu<kychu@qq.com>
  * @brief   public methods for all algorithm module.
  ******************************************************************************
  */
#ifndef __ALGORITHM_H
#define __ALGORITHM_H

#include <math.h>
#include "mtype.h"

#define LIMIT_MAX(x, y)                (((x) > (y)) ? (y) : (x))
#define LIMIT_MIN(x, y)                (((x) < (y)) ? (y) : (x))
#define LIMIT_RANGE(x, max, min)       (LIMIT_MAX(LIMIT_MIN((x), (min)), (max)))

#ifndef MIN
#define MIN(x, y)                      (((x) < (y)) ? (x) : (y))
#endif /* defined(MIN) */
#ifndef MAX
#define MAX(x, y)                      (((x) < (y)) ? (y) : (x))
#endif /* defined(MAX) */

#define ABS(x)                         (((x) > 0) ? (x) : (-(x)))

#define DEG_TO_RAD 0.017453292519943295769236907684886f
#define RAD_TO_DEG 57.295779513082320876798154814105f

#define DEG2RAD(deg)                   ((deg) * 0.017453292519943295769236907684886f)
#define RAD2DEG(rad)                   ((rad) * 57.295779513082320876798154814105f)

void NormalizeVector(Vector3D *v);
float ScalarProduct(Vector3D *va, Vector3D *vb);
float apply_deadband(float value, float deadband);
void step_change(float *in, float target, float step, float deadBand);
float apply_limit(float in, float min, float max);

void fusionQ_6dof(imu_6dof_u *unit, quat_t *q, float prop_gain, float intg_gain, float dt);
void fusionQ_9dof(imu_9dof_u *unit, quat_t *q, float prop_gain, float intg_gain, float dt);
void Quat2Euler(quat_t *q, euler_t *e);

double computeAzimuth(double lat1, double lon1, double lat2, double lon2);

void pid_loop(PID* pid, float expect, float measure);

#endif /* __ALGORITHM_H */

/******************** kyChu<kyChu@qq.com> **** END OF FILE ********************/

