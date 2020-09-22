/**
  ******************************************************************************
  * @file    mtype.h
  * @author  kyChu<kychu@qq.com>
  * @brief   public type for all algorithm module.
  ******************************************************************************
  */

#ifndef _ALGORITHM_MTYPE_H_
#define _ALGORITHM_MTYPE_H_

#include <stdint.h>
#include "sys_def.h"

typedef struct {
  float X, Y;
} Vector2D;

__PACK_BEGIN typedef struct {
  float X, Y, Z;
} __PACK_END Vector3D;

__PACK_BEGIN typedef struct {
  int16_t x, y, z;
} __PACK_END i16_3d_t;

__PACK_BEGIN typedef struct {
  float x, y, z;
} __PACK_END f3d_t;

__PACK_BEGIN typedef struct {
  double x, y, z;
} __PACK_END d3d_t;

typedef struct
{
  float kp, ki, kd, preErr, Output, I_max, I_sum, dt, D_max;
} PID;

__PACK_BEGIN typedef struct {
  i16_3d_t Acc;
  i16_3d_t Gyr;
  i16_3d_t Mag;
  uint16_t Temp;
  uint32_t TS;
} __PACK_END imu_9dof_r;

__PACK_BEGIN typedef struct {
  i16_3d_t Acc;
  i16_3d_t Gyr;
  int16_t Temp;
  uint32_t TS;
} __PACK_END imu_6dof_r;

__PACK_BEGIN typedef struct {
  f3d_t Acc;
  f3d_t Gyr;
  f3d_t Mag;
  float Temp;
  uint32_t TS;
} __PACK_END imu_9dof_u;

__PACK_BEGIN typedef struct {
  f3d_t Acc;
  f3d_t Gyr;
  float Temp;
  uint32_t TS;
} __PACK_END imu_6dof_u;

__PACK_BEGIN typedef struct {
  float qw, qx, qy, qz;
} __PACK_END quat_t;

__PACK_BEGIN typedef struct {
  float pitch;
  float roll;
  float yaw;
} __PACK_END euler_t;

#endif /* _ALGORITHM_MTYPE_H_ */
