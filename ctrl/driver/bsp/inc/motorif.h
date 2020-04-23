/*
 * motorif.h
 *
 *  Created on: Apr 23, 2020
 *      Author: kychu
 */

#ifndef BSP_INC_MOTORIF_H_
#define BSP_INC_MOTORIF_H_

#include "SysConfig.h"

#define MOTOR_TIM                          TIM1
#define MOTOR_TIM_CLK_ENABLE()             __TIM1_CLK_ENABLE()
#define MOTOR_TIM_CHANNEL_A                TIM_CHANNEL_1
#define MOTOR_TIM_CHANNEL_B                TIM_CHANNEL_2

#define MOTOR_TIM_CHANNEL_GPIO_PORT()      __HAL_RCC_GPIOA_CLK_ENABLE();
#define MOTOR_TIM_GPIO_PORT_CHANNEL        GPIOA
#define MOTOR_TIM_GPIO_PIN_CHANNEL_A       GPIO_PIN_8
#define MOTOR_TIM_GPIO_PIN_CHANNEL_B       GPIO_PIN_9
#define MOTOR_TIM_GPIO_AF_CHANNEL          GPIO_AF1_TIM1

status_t motorif_init(void);
status_t motorif_set_dutycycle_a(uint32_t dutycycle);
status_t motorif_set_dutycycle_b(uint32_t dutycycle);

void motorif_msp_init(TIM_HandleTypeDef *htim);

#endif /* BSP_INC_MOTORIF_H_ */
