/*
 * sdif.c
 *
 *  Created on: May 7, 2020
 *      Author: kychu
 */

#include "sdif.h"

/* SPI handler declaration */
static SPI_HandleTypeDef SD_SpiHandle;

#if FREERTOS_ENABLED
static osMutexId if_mutex = NULL;
#else
#endif /* FREERTOS_ENABLED */

status_t sdif_init(void)
{
  /*##-1- Configure the SPI peripheral #######################################*/
  /* Set the SPI parameters */
  SD_SpiHandle.Instance               = SD_SPI;
  SD_SpiHandle.State                  = HAL_SPI_STATE_RESET;
  SD_SpiHandle.Init.Mode              = SPI_MODE_MASTER;
  SD_SpiHandle.Init.Direction         = SPI_DIRECTION_2LINES;
  SD_SpiHandle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4; // 13.5MHz
  SD_SpiHandle.Init.CLKPhase          = SPI_PHASE_2EDGE; // MODE 3
  SD_SpiHandle.Init.CLKPolarity       = SPI_POLARITY_HIGH;
  SD_SpiHandle.Init.DataSize          = SPI_DATASIZE_8BIT;
  SD_SpiHandle.Init.FirstBit          = SPI_FIRSTBIT_MSB;
  SD_SpiHandle.Init.TIMode            = SPI_TIMODE_DISABLE;
  SD_SpiHandle.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;
  SD_SpiHandle.Init.CRCPolynomial     = 7;
  SD_SpiHandle.Init.NSS               = SPI_NSS_SOFT;

  if(HAL_SPI_Init(&SD_SpiHandle) != HAL_OK) return status_error; /* Initialization Error */

#if FREERTOS_ENABLED
  /* Create the mutex  */
  osMutexDef(SDIFMutex);
  if_mutex = osMutexCreate(osMutex(SDIFMutex));
  if(if_mutex == NULL) return status_error;
#else
#endif /* FREERTOS_ENABLED */

  /* SD chip select high */
  sdif_cs_state(1);
  /* Send dummy byte 0xFF, 10 times with CS high */
  /* Rise CS and MOSI for 80 clocks cycles */
  for(uint8_t counter = 0; counter <= 9; counter ++) {
    /* Send dummy byte 0xFF */
    sdif_rxtx_byte(0xFF);
  }

  return status_ok;
}

void sdif_cs_state(uint8_t sta)
{
  if(sta) HAL_GPIO_WritePin(SD_SPI_NSS_GPIO_PORT, SD_SPI_NSS_PIN, GPIO_PIN_SET);//SD_SPI_NSS_GPIO_PORT->BSRR = SD_SPI_NSS_PIN;
  else HAL_GPIO_WritePin(SD_SPI_NSS_GPIO_PORT, SD_SPI_NSS_PIN, GPIO_PIN_RESET);//SD_SPI_NSS_GPIO_PORT->BSRR = (uint32_t)SD_SPI_NSS_PIN << 16;
}

uint8_t sdif_rxtx_byte(uint8_t byte)
{
  uint8_t ret;
#if FREERTOS_ENABLED
  osMutexWait(if_mutex, osWaitForever);
#else
#endif /* FREERTOS_ENABLED */
  HAL_SPI_TransmitReceive(&SD_SpiHandle, &byte, &ret, 1, 1000);
#if FREERTOS_ENABLED
  osMutexRelease(if_mutex);
#else
#endif /* FREERTOS_ENABLED */
  return ret;
}

status_t sdif_txrx_bytes(uint8_t *pTxData, uint8_t *pRxData, uint16_t Size)
{
  status_t ret;
#if FREERTOS_ENABLED
  osMutexWait(if_mutex, osWaitForever);
#else
#endif /* FREERTOS_ENABLED */
  ret = (status_t)HAL_SPI_TransmitReceive(&SD_SpiHandle, pTxData, pRxData, Size, 1000);
#if FREERTOS_ENABLED
  osMutexRelease(if_mutex);
#else
#endif /* FREERTOS_ENABLED */
  return ret;
}

status_t sdif_txrx_bytes_it(uint8_t *pTxData, uint8_t *pRxData, uint16_t Size)
{
#if FREERTOS_ENABLED
  portBASE_TYPE taskWoken = pdFALSE;
  xSemaphoreTakeFromISR(if_mutex, &taskWoken);
#else
#endif /* FREERTOS_ENABLED */
//  sdif_select(0);
  return (status_t)HAL_SPI_TransmitReceive_IT(&SD_SpiHandle, pTxData, pRxData, Size);
}

void sdif_rxtxcplt_callback(SPI_HandleTypeDef *hspi)
{
//  sdif_deselect(0);
#if FREERTOS_ENABLED
  portBASE_TYPE taskWoken = pdFALSE;
  xSemaphoreGiveFromISR(if_mutex, &taskWoken);
#else
#endif /* FREERTOS_ENABLED */
}

void sdif_msp_init(SPI_HandleTypeDef *hspi)
{
  GPIO_InitTypeDef  GPIO_InitStruct;
  /*##-1- Enable peripherals and GPIO Clocks #################################*/
  /* Enable GPIO TX/RX clock */
  SD_SPI_GPIO_CLK_ENABLE();
  SD_DECT_GPIO_CLK_ENABLE();
  /* Enable SD_SPI clock */
  SD_SPI_CLK_ENABLE();

  /*##-2- Configure peripheral GPIO ##########################################*/
  /* SPI NSS GPIO pin configuration */
  GPIO_InitStruct.Pin       = SD_SPI_NSS_PIN;
  GPIO_InitStruct.Mode      = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull      = GPIO_PULLUP;
  GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(SD_SPI_NSS_GPIO_PORT, &GPIO_InitStruct);
  HAL_GPIO_WritePin(SD_SPI_NSS_GPIO_PORT, SD_SPI_NSS_PIN, GPIO_PIN_SET);

  /* SD Detect GPIO pin configuration */
  GPIO_InitStruct.Pin       = SD_DECT_PIN;
  GPIO_InitStruct.Mode      = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull      = GPIO_PULLUP;
  GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(SD_DECT_GPIO_PORT, &GPIO_InitStruct);

  /* SPI SCK GPIO pin configuration */
  GPIO_InitStruct.Pin       = SD_SPI_SCK_PIN;
  GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull      = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = SD_SPI_SCK_AF;
  HAL_GPIO_Init(SD_SPI_SCK_GPIO_PORT, &GPIO_InitStruct);

  /* SPI MISO GPIO pin configuration */
  GPIO_InitStruct.Pin = SD_SPI_MISO_PIN;
  GPIO_InitStruct.Alternate = SD_SPI_MISO_AF;
  HAL_GPIO_Init(SD_SPI_MISO_GPIO_PORT, &GPIO_InitStruct);

  /* SPI MOSI GPIO pin configuration */
  GPIO_InitStruct.Pin = SD_SPI_MOSI_PIN;
  GPIO_InitStruct.Alternate = SD_SPI_MOSI_AF;
  HAL_GPIO_Init(SD_SPI_MOSI_GPIO_PORT, &GPIO_InitStruct);

  /*##-3- Configure the NVIC for SPI #########################################*/
  /* NVIC for SPI */
  HAL_NVIC_SetPriority(SD_SPI_IRQn, INT_PRIO_FLASHIF_IT, 0);
  NVIC_EnableIRQ(SD_SPI_IRQn);
}

void sdif_msp_deinit(SPI_HandleTypeDef *hspi)
{
  /*##-1- Reset peripherals ##################################################*/
  SD_SPI_FORCE_RESET();
  SD_SPI_RELEASE_RESET();

  /*##-2- Disable peripherals and GPIO Clocks ################################*/
  /* Deconfigure SPI NSS */
  HAL_GPIO_DeInit(SD_SPI_NSS_GPIO_PORT, SD_SPI_NSS_PIN);
  /* Deconfigure SPI SCK */
  HAL_GPIO_DeInit(SD_SPI_SCK_GPIO_PORT, SD_SPI_SCK_PIN);
  /* Deconfigure SPI MISO */
  HAL_GPIO_DeInit(SD_SPI_MISO_GPIO_PORT, SD_SPI_MISO_PIN);
  /* Deconfigure SPI MOSI */
  HAL_GPIO_DeInit(SD_SPI_MOSI_GPIO_PORT, SD_SPI_MOSI_PIN);
  /* Deconfigure SD Detect */
  HAL_GPIO_DeInit(SD_DECT_GPIO_PORT, SD_DECT_PIN);
}

/**
  * @brief  This function handles SPI interrupt request.
  * @param  None
  * @retval None
  */
void SD_SPI_IRQHandler(void)
{
  HAL_SPI_IRQHandler(&SD_SpiHandle);
}
