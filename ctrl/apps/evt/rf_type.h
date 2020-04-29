/*
 * rf_type.h
 *
 *  Created on: Apr 27, 2020
 *      Author: kychu
 */

#ifndef EVT_RF_TYPE_H_
#define EVT_RF_TYPE_H_

#include <stdint.h>

typedef enum {
  RF_MODE_0 = 0,
  RF_MODE_1 = 1,
  RF_MODE_2 = 2,
  RF_MODE_3 = 3,
} rf_mode_def;

typedef struct {
  uint16_t Throttle, Pitch, Roll, Yaw;
  rf_mode_def mode;
  uint32_t buttons;
} rf_ctrl_t;

#endif /* EVT_RF_TYPE_H_ */
