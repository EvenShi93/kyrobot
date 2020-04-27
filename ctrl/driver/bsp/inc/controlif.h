#ifndef __CONTROLIF_H
#define __CONTROLIF_H

#include "SysConfig.h"

/* Definition for CTRL_UART clock resources */
#define CTRL_UART                            UART4
#define CTRL_UART_CLK_ENABLE()               __UART4_CLK_ENABLE()
#define CTRL_UART_DMA_CLK_ENABLE()           __HAL_RCC_DMA1_CLK_ENABLE()
#define CTRL_UART_RX_GPIO_CLK_ENABLE()       __GPIOA_CLK_ENABLE()
#define CTRL_UART_TX_GPIO_CLK_ENABLE()       __GPIOA_CLK_ENABLE()

#define CTRL_UART_FORCE_RESET()              __UART4_FORCE_RESET()
#define CTRL_UART_RELEASE_RESET()            __UART4_RELEASE_RESET()

/* Definition for CTRL_UART Pins */
#define CTRL_UART_TX_PIN                     GPIO_PIN_0
#define CTRL_UART_TX_GPIO_PORT               GPIOA
#define CTRL_UART_TX_AF                      GPIO_AF8_UART4
#define CTRL_UART_RX_PIN                     GPIO_PIN_1
#define CTRL_UART_RX_GPIO_PORT               GPIOA
#define CTRL_UART_RX_AF                      GPIO_AF8_UART4

/* Definition for CTRL_UART's DMA */
#define CTRL_UART_TX_DMA_STREAM              DMA1_Stream4
#define CTRL_UART_RX_DMA_STREAM              DMA1_Stream2
#define CTRL_UART_TX_DMA_CHANNEL             DMA_CHANNEL_4
#define CTRL_UART_RX_DMA_CHANNEL             DMA_CHANNEL_4

/* Definition for CTRL_UART's NVIC */
#define CTRL_UART_DMA_TX_IRQn                DMA1_Stream4_IRQn
#define CTRL_UART_DMA_RX_IRQn                DMA1_Stream2_IRQn
#define CTRL_UART_DMA_TX_IRQHandler          DMA1_Stream4_IRQHandler
#define CTRL_UART_DMA_RX_IRQHandler          DMA1_Stream2_IRQHandler

/* Definition for CTRL_UART's NVIC */
#define CTRL_UART_IRQn                       UART4_IRQn
#define CTRL_UART_IRQHandler                 UART4_IRQHandler

#define CTRLIF_CACHE_DEPTH                   (512)

status_t ctrlif_init(void);
uint32_t ctrlif_cache_usage(void);
uint32_t ctrlif_rx_bytes(uint8_t *p, uint32_t l);
status_t ctrlif_tx_bytes(uint8_t *p, uint32_t l);
status_t ctrlif_tx_bytes_util(uint8_t *p, uint32_t l);
status_t ctrlif_tx_string_util(const char *str);

void ctrlif_msp_init(UART_HandleTypeDef *huart);
void ctrlif_msp_deinit(UART_HandleTypeDef *huart);
void ctrlif_txcplt_callback(UART_HandleTypeDef *huart);
void ctrlif_rxcplt_callback(UART_HandleTypeDef *huart);

#endif /* __CONTROLIF_H */
