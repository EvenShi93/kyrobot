/*
 * logconfig.h
 *
 *  Created on: Dec 10, 2019
 *      Author: kychu
 */

#ifndef __LOGCONFIG_H_
#define __LOGCONFIG_H_

#include "SysConfig.h"

#if CONFIG_LOG_ENABLE
#define CONFIG_LOG_CACHE_SIZE                    (128)
#endif /* CONFIG_LOG_ENABLE */

#if CONFIG_LOG_ENABLE
#define CONFIG_LOG_ALERT_ENABLED                 (0)
#define CONFIG_LOG_ERROR_ENABLED                 (1)
#define CONFIG_LOG_WARN_ENABLED                  (1)
#define CONFIG_LOG_INFO_ENABLED                  (1)
#define CONFIG_LOG_DEBUG_ENABLED                 (0)
#define CONFIG_LOG_VERBOSE_ENABLED               (0)

#define CONFIG_LOG_COLORS_ENABLED                (1)
#else
#define CONFIG_LOG_ALERT_ENABLED                 (0)
#define CONFIG_LOG_ERROR_ENABLED                 (0)
#define CONFIG_LOG_WARN_ENABLED                  (0)
#define CONFIG_LOG_INFO_ENABLED                  (0)
#define CONFIG_LOG_DEBUG_ENABLED                 (0)
#define CONFIG_LOG_VERBOSE_ENABLED               (0)

#define CONFIG_LOG_COLORS_ENABLED                (0)
#endif /* CONFIG_LOG_ENABLE */

#endif /* __LOGCONFIG_H_ */
