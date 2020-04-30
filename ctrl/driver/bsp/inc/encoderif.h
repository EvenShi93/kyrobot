/*
 * encoderif.h
 *
 *  Created on: Apr 30, 2020
 *      Author: kychu
 */

#ifndef BSP_INC_ENCODERIF_H_
#define BSP_INC_ENCODERIF_H_

#include "SysConfig.h"

#define ENC_TIM                            TIM3
#define ENC_TIM_CLK_ENABLE()               __TIM3_CLK_ENABLE()

#define ENC_TIM_CHANNEL_GPIO_PORT()        __HAL_RCC_GPIOB_CLK_ENABLE();
#define ENC_TIM_GPIO_PORT_CHANNEL          GPIOB
#define ENC_TIM_GPIO_AF_CHANNEL            GPIO_AF2_TIM3

#define ENC_TIM_GPIO_PIN_CHANNEL1          GPIO_PIN_4
#define ENC_TIM_GPIO_PIN_CHANNEL2          GPIO_PIN_5

status_t encoderif_init(void);
int16_t encoderif_take_counter(void);

void encoderif_msp_init(TIM_HandleTypeDef *htim);

#endif /* BSP_INC_ENCODERIF_H_ */
