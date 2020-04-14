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
#define CONFIG_DEBUG_ALERT                       (0)
#define CONFIG_DEBUG_ERROR                       (1)
#define CONFIG_DEBUG_WARN                        (1)
#define CONFIG_DEBUG_INFO                        (1)

#define CONFIG_LOG_COLORS_ENABLED                (1)
#else
#define CONFIG_DEBUG_ALERT                       (0)
#define CONFIG_DEBUG_ERROR                       (0)
#define CONFIG_DEBUG_WARN                        (0)
#define CONFIG_DEBUG_INFO                        (0)

#define CONFIG_LOG_COLORS_ENABLED                (0)
#endif /* CONFIG_LOG_ENABLE */

#endif /* __LOGCONFIG_H_ */
