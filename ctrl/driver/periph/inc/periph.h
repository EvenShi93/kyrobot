/*
 * periph.h
 *
 *  Created on: May 8, 2020
 *      Author: kychu
 */

#ifndef PERIPH_INC_PERIPH_H_
#define PERIPH_INC_PERIPH_H_

#include "SysConfig.h"

#include "periph_config.h"

#include "usart2.h"
#include "uart4.h"
#include "usart6.h"

status_t periph_disabled(const char *tag);

#endif /* PERIPH_INC_PERIPH_H_ */
