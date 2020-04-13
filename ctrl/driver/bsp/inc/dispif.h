/*
 * dispif.h
 *
 *  Created on: Apr 9, 2020
 *      Author: kychu
 */

#ifndef BSP_INC_DISPIF_H_
#define BSP_INC_DISPIF_H_

#include "SysConfig.h"

#define DISP_SPI                             SPI3
#define DISP_SPI_CLK_ENABLE()                __HAL_RCC_SPI3_CLK_ENABLE()
#define DISP_SPI_DMA_CLK_ENABLE()            __HAL_RCC_DMA1_CLK_ENABLE()

#define DISP_GPIO_CLK_ENABLE()               do { __HAL_RCC_GPIOC_CLK_ENABLE(); } while(0)

#define DISP_SPI_FORCE_RESET()               __HAL_RCC_SPI3_FORCE_RESET()
#define DISP_SPI_RELEASE_RESET()             __HAL_RCC_SPI3_RELEASE_RESET()

/* Definition for DISP_SPI Pins */
#define DISP_SPI_SCK_PIN                     GPIO_PIN_10
#define DISP_SPI_SCK_GPIO_PORT               GPIOC
#define DISP_SPI_SCK_AF                      GPIO_AF6_SPI3
#define DISP_SPI_MOSI_PIN                    GPIO_PIN_12
#define DISP_SPI_MOSI_GPIO_PORT              GPIOC
#define DISP_SPI_MOSI_AF                     GPIO_AF6_SPI3

#define DISP_SPI_NSS_PIN                     GPIO_PIN_11
#define DISP_SPI_NSS_GPIO_PORT               GPIOC

/* Definition for DISP_SPI's DMA */
#define DISP_SPI_TX_DMA_STREAM               DMA1_Stream7
#define DISP_SPI_TX_DMA_CHANNEL              DMA_CHANNEL_0

/* Definition for DISP_SPI's NVIC */
#define DISP_SPI_DMA_TX_IRQn                 DMA1_Stream7_IRQn

#define DISP_SPI_DMA_TX_IRQHandler           DMA1_Stream7_IRQHandler

status_t dispif_init(void);
status_t dispif_tx_bytes(uint8_t *pTxData, uint16_t Size);
status_t dispif_tx_bytes_dma(uint8_t *pTxData, uint16_t Size);

void dispif_msp_init(SPI_HandleTypeDef *hspi);
void dispif_msp_deinit(SPI_HandleTypeDef *hspi);
void dispif_txcplt_callback(SPI_HandleTypeDef *hspi);

#endif /* BSP_INC_DISPIF_H_ */
