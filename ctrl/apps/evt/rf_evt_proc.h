/*
 * rf_evt_proc.h
 *
 *  Created on: Apr 29, 2020
 *      Author: kychu
 */

#ifndef EVT_RF_EVT_PROC_H_
#define EVT_RF_EVT_PROC_H_

#include "rf_type.h"

uint16_t rmt_get_thr_chan(void);
uint16_t rmt_get_yaw_chan(void);
uint16_t rmt_get_pit_chan(void);
uint16_t rmt_get_rol_chan(void);

uint32_t rmt_is_signallost(void);

void rmt_get_channels(rf_ctrl_t *rf);

#endif /* EVT_RF_EVT_PROC_H_ */
