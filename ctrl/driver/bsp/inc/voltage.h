/*
 * voltage.h
 *
 *  Created on: Apr 24, 2020
 *      Author: kychu
 */

#ifndef BSP_INC_VOLTAGE_H_
#define BSP_INC_VOLTAGE_H_

#include "SysConfig.h"

#define BAT_ADC                                  ADC1
#define BAT_ADC_CLK_ENABLE()                     __HAL_RCC_ADC1_CLK_ENABLE()
#define BAT_ADC_DMA_CLK_ENABLE()                 __HAL_RCC_DMA2_CLK_ENABLE()
#define BAT_ADC_CHANNEL_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOB_CLK_ENABLE()

#define BAT_ADC_FORCE_RESET()                    __HAL_RCC_ADC_FORCE_RESET()
#define BAT_ADC_RELEASE_RESET()                  __HAL_RCC_ADC_RELEASE_RESET()

/* Definition for BAT_ADC Channel Pin */
#define BAT_ADC_CHANNEL_PIN                      GPIO_PIN_1
#define BAT_ADC_CHANNEL_GPIO_PORT                GPIOB

/* Definition for ADCx's Channel */
#define BAT_ADC_CHANNEL                          ADC_CHANNEL_9

/* Definition for ADCx's DMA */
#define BAT_ADC_DMA_CHANNEL                      DMA_CHANNEL_0
#define BAT_ADC_DMA_STREAM                       DMA2_Stream4

/* Definition for ADCx's NVIC */
#define BAT_ADC_DMA_IRQn                         DMA2_Stream4_IRQn
#define BAT_ADC_DMA_IRQHandler                   DMA2_Stream4_IRQHandler

status_t voltage_measure_init(void);
uint16_t voltage_get_measurement(void);

void bat_adc_msp_init(ADC_HandleTypeDef *hadc);
void bat_adc_msp_deinit(ADC_HandleTypeDef *hadc);

#endif /* BSP_INC_VOLTAGE_H_ */
