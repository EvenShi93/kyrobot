/*
 * sdif.h
 *
 *  Created on: May 7, 2020
 *      Author: kychu
 */

#ifndef BSP_INC_SDIF_H_
#define BSP_INC_SDIF_H_

#include "SysConfig.h"

#define SD_SPI                             SPI2
#define SD_SPI_CLK_ENABLE()                __HAL_RCC_SPI2_CLK_ENABLE()
#define SD_SPI_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOB_CLK_ENABLE()

#define SD_SPI_FORCE_RESET()               __HAL_RCC_SPI2_FORCE_RESET()
#define SD_SPI_RELEASE_RESET()             __HAL_RCC_SPI2_RELEASE_RESET()

/* Definition for SD_SPI Pins */
#define SD_SPI_SCK_PIN                     GPIO_PIN_13
#define SD_SPI_SCK_GPIO_PORT               GPIOB
#define SD_SPI_SCK_AF                      GPIO_AF5_SPI2
#define SD_SPI_MISO_PIN                    GPIO_PIN_14
#define SD_SPI_MISO_GPIO_PORT              GPIOB
#define SD_SPI_MISO_AF                     GPIO_AF5_SPI2
#define SD_SPI_MOSI_PIN                    GPIO_PIN_15
#define SD_SPI_MOSI_GPIO_PORT              GPIOB
#define SD_SPI_MOSI_AF                     GPIO_AF5_SPI2

#define SD_SPI_NSS_PIN                     GPIO_PIN_12
#define SD_SPI_NSS_GPIO_PORT               GPIOB

#define SD_DECT_GPIO_CLK_ENABLE()          __HAL_RCC_GPIOB_CLK_ENABLE()
#define SD_DECT_PIN                        GPIO_PIN_2
#define SD_DECT_GPIO_PORT                  GPIOB

/* Definition for SD_SPI's NVIC */
#define SD_SPI_IRQn                        SPI2_IRQn
#define SD_SPI_IRQHandler                  SPI2_IRQHandler

status_t sdif_init(void);
void sdif_cs_state(uint8_t sta);
uint8_t sdif_rxtx_byte(uint8_t byte);
status_t sdif_txrx_bytes(uint8_t *pTxData, uint8_t *pRxData, uint16_t Size);
status_t sdif_txrx_bytes_it(uint8_t *pTxData, uint8_t *pRxData, uint16_t Size);

void sdif_msp_init(SPI_HandleTypeDef *hspi);
void sdif_msp_deinit(SPI_HandleTypeDef *hspi);
void sdif_rxtxcplt_callback(SPI_HandleTypeDef *hspi);

#endif /* BSP_INC_SDIF_H_ */
