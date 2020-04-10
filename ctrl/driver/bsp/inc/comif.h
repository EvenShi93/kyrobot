#ifndef __COMIF_H
#define __COMIF_H

#include "SysConfig.h"

/* Definition for COM_UART clock resources */
#define COM_UART                            USART6
#define COM_UART_CLK_ENABLE()               __USART6_CLK_ENABLE()
#define COM_UART_DMA_CLK_ENABLE()           __HAL_RCC_DMA2_CLK_ENABLE()
#define COM_UART_RX_GPIO_CLK_ENABLE()       __GPIOC_CLK_ENABLE()
#define COM_UART_TX_GPIO_CLK_ENABLE()       __GPIOC_CLK_ENABLE()

#define COM_UART_FORCE_RESET()              __USART6_FORCE_RESET()
#define COM_UART_RELEASE_RESET()            __USART6_RELEASE_RESET()

/* Definition for COM_UART Pins */
#define COM_UART_TX_PIN                     GPIO_PIN_6
#define COM_UART_TX_GPIO_PORT               GPIOC
#define COM_UART_TX_AF                      GPIO_AF8_USART6
#define COM_UART_RX_PIN                     GPIO_PIN_7
#define COM_UART_RX_GPIO_PORT               GPIOC
#define COM_UART_RX_AF                      GPIO_AF8_USART6

/* Definition for COM_UART's DMA */
#define COM_UART_TX_DMA_STREAM              DMA2_Stream6
#define COM_UART_RX_DMA_STREAM              DMA2_Stream2
#define COM_UART_TX_DMA_CHANNEL             DMA_CHANNEL_5
#define COM_UART_RX_DMA_CHANNEL             DMA_CHANNEL_5

/* Definition for COM_UART's NVIC */
#define COM_UART_DMA_TX_IRQn                DMA2_Stream6_IRQn
#define COM_UART_DMA_RX_IRQn                DMA2_Stream2_IRQn
#define COM_UART_DMA_TX_IRQHandler          DMA2_Stream6_IRQHandler
#define COM_UART_DMA_RX_IRQHandler          DMA2_Stream2_IRQHandler

/* Definition for COM_UART's NVIC */
#define COM_UART_IRQn                       USART6_IRQn
#define COM_UART_IRQHandler                 USART6_IRQHandler

#define COMIF_CACHE_DEPTH                   (256)

status_t comif_init(void);
status_t comif_set_baudrate(uint32_t baudrate);
uint32_t comif_rx_bytes(uint8_t *p, uint32_t l);
status_t comif_tx_bytes(uint8_t *p, uint32_t l);
status_t comif_tx_bytes_util(uint8_t *p, uint32_t l);
status_t comif_tx_string_util(const char *p);

void comif_msp_init(UART_HandleTypeDef *huart);
void comif_msp_deinit(UART_HandleTypeDef *huart);
void comif_txcplt_callback(UART_HandleTypeDef *huart);
void comif_rxcplt_callback(UART_HandleTypeDef *huart);

#endif /* __COMIF_H */
