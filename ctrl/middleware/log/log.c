/*
 * log.c
 *
 *  Created on: Dec 10, 2019
 *      Author: kychu
 */

#include "log.h"

#if CONFIG_LOG_ENABLE

#include <stdio.h>
#include <stdarg.h>

#if FREERTOS_ENABLED
#include "cmsis_os.h"
#endif /* FREERTOS_ENABLED */

#if FREERTOS_ENABLED
static char *log_cache = NULL;
#else
static char log_cache[CONFIG_LOG_CACHE_SIZE];
#endif /* FREERTOS_ENABLED */
static log_put_t log_put_func = NULL;
#if FREERTOS_ENABLED
static osMutexId logMutex = NULL;
#endif /* FREERTOS_ENABLED */

/****************************************************************************
 * Public Functions
 ****************************************************************************/

status_t log_init(log_put_t ptx)
{
#if FREERTOS_ENABLED
  log_cache = kmm_alloc(CONFIG_LOG_CACHE_SIZE);
  if(log_cache == NULL) return status_nomem;

  /* Create the mutex  */
  osMutexDef(LOGMutex);
  logMutex = osMutexCreate(osMutex(LOGMutex));
  if(logMutex == NULL) return status_error;
#endif /* FREERTOS_ENABLED */

  if(ptx == NULL) return status_error;
  log_put_func = ptx;

  return status_ok;
}

status_t log_write(const char *format, ...)
{
  if(log_cache == NULL) return status_error;
  if(log_put_func == NULL) return status_error;
  va_list ap;

  va_start(ap, format);
#if FREERTOS_ENABLED
  osMutexWait(logMutex, osWaitForever);
#endif /* FREERTOS_ENABLED */
  vsprintf(log_cache, format, ap);
  log_put_func((const char *)log_cache);
#if FREERTOS_ENABLED
  osMutexRelease(logMutex);
#endif /* FREERTOS_ENABLED */
  va_end(ap);

  return status_ok;
}

__weak uint32_t log_timestamp(void)
{
#if FREERTOS_ENABLED
  if (xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED) {
    return 0;
  }

  return xTaskGetTickCount() * (1000 / configTICK_RATE_HZ);
#else
  return 0;
#endif /* FREERTOS_ENABLED */
}

#endif /* CONFIG_LOG_ENABLE */
