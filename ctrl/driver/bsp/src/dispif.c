/*
 * dispif.c
 *
 *  Created on: Apr 9, 2020
 *      Author: kychu
 */

#include "dispif.h"
#include "gpios.h"

/* SPI handler declaration */
static SPI_HandleTypeDef Disp_SpiHandle;

static DMA_HandleTypeDef hdma_tx;

#if FREERTOS_ENABLED
static osMutexId if_mutex = NULL;
#else
#endif /* FREERTOS_ENABLED */

#define DISP_SPI_CS_SELECT()    HAL_GPIO_WritePin(DISP_SPI_NSS_GPIO_PORT, DISP_SPI_NSS_PIN, GPIO_PIN_RESET)
#define DISP_SPI_CS_DESELECT()  HAL_GPIO_WritePin(DISP_SPI_NSS_GPIO_PORT, DISP_SPI_NSS_PIN, GPIO_PIN_SET)

status_t dispif_init(void)
{
  /*##-1- Configure the SPI peripheral #######################################*/
  /* Set the SPI parameters */
  Disp_SpiHandle.Instance               = DISP_SPI;
  Disp_SpiHandle.Init.Mode              = SPI_MODE_MASTER;
  Disp_SpiHandle.Init.Direction         = SPI_CR1_BIDIOE | SPI_CR1_BIDIMODE; // TX ONLY
  Disp_SpiHandle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
  Disp_SpiHandle.Init.CLKPhase          = SPI_PHASE_2EDGE; // MODE 3
  Disp_SpiHandle.Init.CLKPolarity       = SPI_POLARITY_HIGH;
  Disp_SpiHandle.Init.DataSize          = SPI_DATASIZE_8BIT;
  Disp_SpiHandle.Init.FirstBit          = SPI_FIRSTBIT_MSB;
  Disp_SpiHandle.Init.TIMode            = SPI_TIMODE_DISABLE;
  Disp_SpiHandle.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;
  Disp_SpiHandle.Init.CRCPolynomial     = 7;
  Disp_SpiHandle.Init.NSS               = SPI_NSS_SOFT;

  if(HAL_SPI_Init(&Disp_SpiHandle) != HAL_OK) return status_error; /* Initialization Error */

#if FREERTOS_ENABLED
  /* Create the mutex  */
  osMutexDef(DISPIFMutex);
  if_mutex = osMutexCreate(osMutex(DISPIFMutex));
  if(if_mutex == NULL) return status_error;
#else
#endif /* FREERTOS_ENABLED */

  return status_ok;
}

status_t dispif_tx_bytes(uint8_t *pTxData, uint16_t Size)
{
  status_t ret;
#if FREERTOS_ENABLED
  osMutexWait(if_mutex, osWaitForever);
#else
#endif /* FREERTOS_ENABLED */
  DISP_SPI_CS_SELECT();
  ret = (status_t)HAL_SPI_Transmit(&Disp_SpiHandle, pTxData, Size, Size);
  DISP_SPI_CS_DESELECT();
#if FREERTOS_ENABLED
  osMutexRelease(if_mutex);
#else
#endif /* FREERTOS_ENABLED */
  return ret;
}

status_t dispif_tx_bytes_dma(uint8_t *pTxData, uint16_t Size)
{
#if FREERTOS_ENABLED
  portBASE_TYPE taskWoken = pdFALSE;
  xSemaphoreTakeFromISR(if_mutex, &taskWoken);
#else
#endif /* FREERTOS_ENABLED */
  DISP_SPI_CS_SELECT();
  return (status_t)HAL_SPI_Transmit_DMA(&Disp_SpiHandle, pTxData, Size);
}

void dispif_txcplt_callback(SPI_HandleTypeDef *hspi)
{
  DISP_SPI_CS_DESELECT();
#if FREERTOS_ENABLED
  portBASE_TYPE taskWoken = pdFALSE;
  xSemaphoreGiveFromISR(if_mutex, &taskWoken);
#else
#endif /* FREERTOS_ENABLED */
}

void dispif_msp_init(SPI_HandleTypeDef *hspi)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  /*##-1- Enable peripherals and GPIO Clocks #################################*/
  /* Enable GPIO clock */
  DISP_GPIO_CLK_ENABLE();
  /* Enable SPI1 clock */
  DISP_SPI_CLK_ENABLE();
  /* Enable DMA clock */
  DISP_SPI_DMA_CLK_ENABLE();

  /*##-2- Configure peripheral GPIO ##########################################*/
  /* SPI NSS GPIO pin configuration */
  GPIO_InitStruct.Pin       = DISP_SPI_NSS_PIN;
  GPIO_InitStruct.Mode      = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull      = GPIO_PULLUP;
  GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(DISP_SPI_NSS_GPIO_PORT, &GPIO_InitStruct);
  HAL_GPIO_WritePin(DISP_SPI_NSS_GPIO_PORT, DISP_SPI_NSS_PIN, GPIO_PIN_SET);

  /* SPI SCK GPIO pin configuration */
  GPIO_InitStruct.Pin       = DISP_SPI_SCK_PIN;
  GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull      = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = DISP_SPI_SCK_AF;
  HAL_GPIO_Init(DISP_SPI_SCK_GPIO_PORT, &GPIO_InitStruct);

  /* SPI MOSI GPIO pin configuration */
  GPIO_InitStruct.Pin = DISP_SPI_MOSI_PIN;
  GPIO_InitStruct.Alternate = DISP_SPI_MOSI_AF;
  HAL_GPIO_Init(DISP_SPI_MOSI_GPIO_PORT, &GPIO_InitStruct);

  /*##-3- Configure the DMA ##################################################*/
  /* Configure the DMA handler for Transmission process */
  hdma_tx.Instance                 = DISP_SPI_TX_DMA_STREAM;
  hdma_tx.Init.Channel             = DISP_SPI_TX_DMA_CHANNEL;
  hdma_tx.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;
  hdma_tx.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
  hdma_tx.Init.MemBurst            = DMA_MBURST_INC4;
  hdma_tx.Init.PeriphBurst         = DMA_PBURST_INC4;
  hdma_tx.Init.Direction           = DMA_MEMORY_TO_PERIPH;
  hdma_tx.Init.PeriphInc           = DMA_PINC_DISABLE;
  hdma_tx.Init.MemInc              = DMA_MINC_ENABLE;
  hdma_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  hdma_tx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
  hdma_tx.Init.Mode                = DMA_NORMAL;
  hdma_tx.Init.Priority            = DMA_PRIORITY_MEDIUM;

  HAL_DMA_Init(&hdma_tx);

  /* Associate the initialized DMA handle to the the SPI handle */
  __HAL_LINKDMA(hspi, hdmatx, hdma_tx);

  /*##-4- Configure the NVIC for DMA #########################################*/
  /* NVIC configuration for DMA transfer complete interrupt (IMU_SPI_TX) */
  HAL_NVIC_SetPriority(DISP_SPI_DMA_TX_IRQn, INT_PRIO_DISPIF_DMATX, 0);
  HAL_NVIC_EnableIRQ(DISP_SPI_DMA_TX_IRQn);
}

void dispif_msp_deinit(SPI_HandleTypeDef *hspi)
{
  /*##-1- Reset peripherals ##################################################*/
  DISP_SPI_FORCE_RESET();
  DISP_SPI_RELEASE_RESET();

  /*##-2- Disable peripherals and GPIO Clocks ################################*/
  /* Deconfigure SPI NSS */
  HAL_GPIO_DeInit(DISP_SPI_NSS_GPIO_PORT, DISP_SPI_NSS_PIN);
  /* Deconfigure SPI SCK */
  HAL_GPIO_DeInit(DISP_SPI_SCK_GPIO_PORT, DISP_SPI_SCK_PIN);
  /* Deconfigure SPI MOSI */
  HAL_GPIO_DeInit(DISP_SPI_MOSI_GPIO_PORT, DISP_SPI_MOSI_PIN);

  /*##-3- Disable the DMA ####################################################*/
  /* De-Initialize the DMA associated to transmission process */
  HAL_DMA_DeInit(&hdma_tx);

  /*##-4- Disable the NVIC for DMA ###########################################*/
  HAL_NVIC_DisableIRQ(DISP_SPI_DMA_TX_IRQn);
}

/**
  * @brief  This function handles DMA TX interrupt request.
  * @param  None
  * @retval None
  */
void DISP_SPI_DMA_TX_IRQHandler(void)
{
  HAL_DMA_IRQHandler(Disp_SpiHandle.hdmatx);
}
