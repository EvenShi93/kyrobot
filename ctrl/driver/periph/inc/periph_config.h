/*
 * periph_config.h
 *
 *  Created on: May 8, 2020
 *      Author: kychu
 */

#ifndef _PERIPH_CONFIG_H_
#define _PERIPH_CONFIG_H_

#include "config.h"

#define UART_HAL_TRANSMIT_TIMEOUT           (1000)
#define USART_HAL_TRANSMIT_TIMEOUT          UART_HAL_TRANSMIT_TIMEOUT

/*## PERIPHERAL DEFINITIONS FOR USART1 #################################*/
/* Definition for USART1 clock resources */
#if USART1_ENABLE
#define USART1_GPIO_CLK_ENABLE()            __GPIOB_CLK_ENABLE()
#endif /* USART1_ENABLE */

/* Definition for USART1 Pins */
#if USART1_TX_ENABLE
#define USART1_TX_PIN                       GPIO_PIN_6
#define USART1_TX_GPIO_PORT                 GPIOB
#define USART1_TX_AF                        GPIO_AF7_USART1
#endif /* USART1_TX_ENABLE */
#if USART1_RX_ENABLE
#define USART1_RX_PIN                       GPIO_PIN_7
#define USART1_RX_GPIO_PORT                 GPIOB
#define USART1_RX_AF                        GPIO_AF7_USART1
#endif /* USART1_RX_ENABLE */

#if USART1_DMA_TX_ENABLE
#define USART1_DMA_TX_CLK_ENABLE()          __HAL_RCC_DMA2_CLK_ENABLE()
#define USART1_TX_DMA_STREAM                DMA2_Stream7
#define USART1_TX_DMA_CHANNEL               DMA_CHANNEL_4
#define USART1_DMA_TX_IRQn                  DMA2_Stream7_IRQn
#define USART1_DMA_TX_IRQHandler            DMA2_Stream7_IRQHandler
#endif /* USART1_DMA_TX_ENABLE */

#if USART1_DMA_RX_ENABLE
#define USART1_DMA_RX_CLK_ENABLE()          __HAL_RCC_DMA2_CLK_ENABLE()
#define USART1_RX_DMA_STREAM                DMA2_Stream5
#define USART1_RX_DMA_CHANNEL               DMA_CHANNEL_4
#define USART1_DMA_RX_IRQn                  DMA2_Stream5_IRQn
#define USART1_DMA_RX_IRQHandler            DMA2_Stream5_IRQHandler
#endif /* USART1_DMA_RX_ENABLE */

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

/*## PERIPHERAL DEFINITIONS FOR UART4 #################################*/
/* Definition for UART4 clock resources */
#if UART4_ENABLE
#define UART4_GPIO_CLK_ENABLE()            __GPIOA_CLK_ENABLE()
#endif /* UART4_ENABLE */

/* Definition for UART4 Pins */
#if UART4_TX_ENABLE
#define UART4_TX_PIN                       GPIO_PIN_0
#define UART4_TX_GPIO_PORT                 GPIOA
#define UART4_TX_AF                        GPIO_AF8_UART4
#endif /* UART4_TX_ENABLE */
#if UART4_RX_ENABLE
#define UART4_RX_PIN                       GPIO_PIN_1
#define UART4_RX_GPIO_PORT                 GPIOA
#define UART4_RX_AF                        GPIO_AF8_UART4
#endif /* UART4_RX_ENABLE */

#if UART4_DMA_TX_ENABLE
#define UART4_DMA_TX_CLK_ENABLE()          __HAL_RCC_DMA1_CLK_ENABLE()
#define UART4_TX_DMA_STREAM                DMA1_Stream4
#define UART4_TX_DMA_CHANNEL               DMA_CHANNEL_4
#define UART4_DMA_TX_IRQn                  DMA1_Stream4_IRQn
#define UART4_DMA_TX_IRQHandler            DMA1_Stream4_IRQHandler
#endif /* UART4_DMA_TX_ENABLE */

#if UART4_DMA_RX_ENABLE
#define UART4_DMA_RX_CLK_ENABLE()          __HAL_RCC_DMA1_CLK_ENABLE()
#define UART4_RX_DMA_STREAM                DMA1_Stream2
#define UART4_RX_DMA_CHANNEL               DMA_CHANNEL_4
#define UART4_DMA_RX_IRQn                  DMA1_Stream2_IRQn
#define UART4_DMA_RX_IRQHandler            DMA1_Stream2_IRQHandler
#endif /* UART4_DMA_RX_ENABLE */

/*## PERIPHERAL DEFINITIONS FOR USART6 #################################*/
/* Definition for USART6 clock resources */
#if USART6_ENABLE
#define USART6_GPIO_CLK_ENABLE()            __GPIOC_CLK_ENABLE()
#endif /* USART6_ENABLE */

/* Definition for USART6 Pins */
#if USART6_TX_ENABLE
#define USART6_TX_PIN                       GPIO_PIN_6
#define USART6_TX_GPIO_PORT                 GPIOC
#define USART6_TX_AF                        GPIO_AF8_USART6
#endif /* USART6_TX_ENABLE */
#if USART6_RX_ENABLE
#define USART6_RX_PIN                       GPIO_PIN_7
#define USART6_RX_GPIO_PORT                 GPIOC
#define USART6_RX_AF                        GPIO_AF8_USART6
#endif /* USART6_RX_ENABLE */

#if USART6_DMA_TX_ENABLE
#define USART6_DMA_TX_CLK_ENABLE()          __HAL_RCC_DMA2_CLK_ENABLE()
#define USART6_TX_DMA_STREAM                DMA2_Stream6
#define USART6_TX_DMA_CHANNEL               DMA_CHANNEL_5
#define USART6_DMA_TX_IRQn                  DMA2_Stream6_IRQn
#define USART6_DMA_TX_IRQHandler            DMA2_Stream6_IRQHandler
#endif /* USART6_DMA_TX_ENABLE */

#if USART6_DMA_RX_ENABLE
#define USART6_DMA_RX_CLK_ENABLE()          __HAL_RCC_DMA2_CLK_ENABLE()
#define USART6_RX_DMA_STREAM                DMA2_Stream2
#define USART6_RX_DMA_CHANNEL               DMA_CHANNEL_5
#define USART6_DMA_RX_IRQn                  DMA2_Stream2_IRQn
#define USART6_DMA_RX_IRQHandler            DMA2_Stream2_IRQHandler
#endif /* USART6_DMA_RX_ENABLE */

#endif /* _PERIPH_CONFIG_H_ */
