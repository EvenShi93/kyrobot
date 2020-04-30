/*
 * notifytimer.h
 *
 *  Created on: Apr 30, 2020
 *      Author: kychu
 */

#ifndef BSP_INC_NOTIFYTIMER_H_
#define BSP_INC_NOTIFYTIMER_H_

#include "SysConfig.h"

typedef void (*NotifyTimerCallback)(void);

status_t notify_timer_init(uint32_t period_ms, NotifyTimerCallback cb);

#endif /* BSP_INC_NOTIFYTIMER_H_ */
