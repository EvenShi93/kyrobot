/*
 * steerif.h
 *
 *  Created on: Apr 22, 2020
 *      Author: kychu
 */

#ifndef BSP_INC_STEERIF_H_
#define BSP_INC_STEERIF_H_

#include "SysConfig.h"

#define STEER_TIM                          TIM8
#define STEER_TIM_CLK_ENABLE()             __TIM8_CLK_ENABLE()
#define STEER_TIM_CHANNEL                  TIM_CHANNEL_4

#define STEER_TIM_CHANNEL_GPIO_PORT()      __HAL_RCC_GPIOC_CLK_ENABLE();
#define STEER_TIM_GPIO_PORT_CHANNEL4       GPIOC
#define STEER_TIM_GPIO_PIN_CHANNEL4        GPIO_PIN_9
#define STEER_TIM_GPIO_AF_CHANNEL4         GPIO_AF3_TIM8

status_t steerif_init(uint32_t period_us, uint32_t poswidth_us);
status_t steerif_set_poswidth(uint32_t poswidth_us);
void steerif_msp_init(TIM_HandleTypeDef *htim);

#endif /* BSP_INC_STEERIF_H_ */
