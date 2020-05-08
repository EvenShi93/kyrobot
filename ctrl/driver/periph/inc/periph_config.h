/*
 * periph_config.h
 *
 *  Created on: May 8, 2020
 *      Author: kychu
 */

#ifndef _PERIPH_CONFIG_H_
#define _PERIPH_CONFIG_H_

#include "config.h"

/*## PERIPHERAL DEFINITIONS FOR USART2 #################################*/
/* Definition for USART2 clock resources */
#if USART2_ENABLE
#define USART2_GPIO_CLK_ENABLE()            __GPIOA_CLK_ENABLE()
#endif /* USART2_ENABLE */

/* Definition for USART2 Pins */
#if USART2_TX_ENABLE
#define USART2_TX_PIN                       GPIO_PIN_2
#define USART2_TX_GPIO_PORT                 GPIOA
#define USART2_TX_AF                        GPIO_AF7_USART2
#endif /* USART2_TX_ENABLE */
#if USART2_RX_ENABLE
#define USART2_RX_PIN                       GPIO_PIN_3
#define USART2_RX_GPIO_PORT                 GPIOA
#define USART2_RX_AF                        GPIO_AF7_USART2
#endif /* USART2_RX_ENABLE */

#if USART2_DMA_TX_ENABLE
#define USART2_DMA_TX_CLK_ENABLE()          __HAL_RCC_DMA1_CLK_ENABLE()
#define USART2_TX_DMA_STREAM                DMA1_Stream6
#define USART2_TX_DMA_CHANNEL               DMA_CHANNEL_4
#define USART2_DMA_TX_IRQn                  DMA1_Stream6_IRQn
#define USART2_DMA_TX_IRQHandler            DMA1_Stream6_IRQHandler
#endif /* USART2_DMA_TX_ENABLE */

#if USART2_DMA_RX_ENABLE
#define USART2_DMA_RX_CLK_ENABLE()          __HAL_RCC_DMA1_CLK_ENABLE()
#define USART2_RX_DMA_STREAM                DMA1_Stream5
#define USART2_RX_DMA_CHANNEL               DMA_CHANNEL_4
#define USART2_DMA_RX_IRQn                  DMA1_Stream5_IRQn
#define USART2_DMA_RX_IRQHandler            DMA1_Stream5_IRQHandler
#endif /* USART2_DMA_RX_ENABLE */

#endif /* _PERIPH_CONFIG_H_ */
