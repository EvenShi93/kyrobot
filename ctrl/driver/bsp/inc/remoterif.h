#ifndef __REMOTERIF_H
#define __REMOTERIF_H

#include "SysConfig.h"

/* Definition for RMT_UART clock resources */
#define RMT_UART                            USART2
#define RMT_UART_CLK_ENABLE()               __USART2_CLK_ENABLE()
#define RMT_UART_DMA_CLK_ENABLE()           __HAL_RCC_DMA1_CLK_ENABLE()
#define RMT_UART_RX_GPIO_CLK_ENABLE()       __GPIOA_CLK_ENABLE()
#define RMT_UART_TX_GPIO_CLK_ENABLE()       __GPIOA_CLK_ENABLE()

#define RMT_UART_FORCE_RESET()              __USART2_FORCE_RESET()
#define RMT_UART_RELEASE_RESET()            __USART2_RELEASE_RESET()

/* Definition for RMT_UART Pins */
#define RMT_UART_TX_PIN                     GPIO_PIN_2
#define RMT_UART_TX_GPIO_PORT               GPIOA
#define RMT_UART_TX_AF                      GPIO_AF7_USART2
#define RMT_UART_RX_PIN                     GPIO_PIN_3
#define RMT_UART_RX_GPIO_PORT               GPIOA
#define RMT_UART_RX_AF                      GPIO_AF7_USART2

/* Definition for RMT_UART's DMA */
#define RMT_UART_TX_DMA_STREAM              DMA1_Stream6
#define RMT_UART_RX_DMA_STREAM              DMA1_Stream5
#define RMT_UART_TX_DMA_CHANNEL             DMA_CHANNEL_4
#define RMT_UART_RX_DMA_CHANNEL             DMA_CHANNEL_4

/* Definition for RMT_UART's NVIC */
#define RMT_UART_DMA_TX_IRQn                DMA1_Stream6_IRQn
#define RMT_UART_DMA_RX_IRQn                DMA1_Stream5_IRQn
#define RMT_UART_DMA_TX_IRQHandler          DMA1_Stream6_IRQHandler
#define RMT_UART_DMA_RX_IRQHandler          DMA1_Stream5_IRQHandler

/* Definition for RMT_UART's NVIC */
#define RMT_UART_IRQn                       USART2_IRQn
#define RMT_UART_IRQHandler                 USART2_IRQHandler

#define RMTIF_CACHE_DEPTH                   (256)

status_t rmtif_init(void);
status_t rmtif_set_baudrate(uint32_t baudrate);
uint32_t rmtif_rx_bytes(uint8_t *p, uint32_t l);
status_t rmtif_tx_bytes(uint8_t *p, uint32_t l);
status_t rmtif_tx_bytes_util(uint8_t *p, uint32_t l);

void rmtif_msp_init(UART_HandleTypeDef *huart);
void rmtif_msp_deinit(UART_HandleTypeDef *huart);
void rmtif_txcplt_callback(UART_HandleTypeDef *huart);
void rmtif_rxcplt_callback(UART_HandleTypeDef *huart);

#endif /* __REMOTERIF_H */
