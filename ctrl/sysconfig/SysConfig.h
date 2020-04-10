#ifndef __SYSCONFIG_H
#define __SYSCONFIG_H

#include "stm32f7xx_hal.h"

#include "SysDataTypes.h"

#include "cmsis_os.h"

#include "sys_def.h"

#include "log.h"

#include "config.h"

#if FREERTOS_ENABLED
#define kmm_alloc                                pvPortMalloc
#define kmm_free                                 vPortFree
#else
#include <stdlib.h>

#define kmm_alloc                                malloc
#define kmm_free                                 free
#endif /* FREERTOS_ENABLED */

#if FREERTOS_ENABLED
#define delay                               osDelay
#else
#define delay                               HAL_Delay
#endif /* FREERTOS_ENABLED */

#define UNUSED_VARIABLE(X)                       ((void)(X))
#define UNUSED_PARAMETER(X)                      UNUSED_VARIABLE(X)
#define UNUSED_RETURN_VALUE(X)                   UNUSED_VARIABLE(X)

void APP_StartThread(void const *argument);

#endif /* __SYSCONFIG_H */

/******************** kyChu<kyChu@qq.com> **** END OF FILE ********************/
