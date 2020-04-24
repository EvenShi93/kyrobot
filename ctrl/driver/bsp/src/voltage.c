/*
 * voltage.c
 *
 *  Created on: Apr 24, 2020
 *      Author: kychu
 */

#include "voltage.h"

/* ADC handler declaration */
static ADC_HandleTypeDef    AdcHandle;

static DMA_HandleTypeDef    hdma_adc;

/* Variable used to get converted value */
__IO static uint16_t uhADCxConvertedValue = 0;

status_t voltage_measure_init(void)
{
  ADC_ChannelConfTypeDef sConfig;
  /*##-1- Configure the ADC peripheral #######################################*/
  AdcHandle.Instance          = BAT_ADC;
  AdcHandle.State             = HAL_ADC_STATE_RESET;

  AdcHandle.Init.ClockPrescaler        = ADC_CLOCKPRESCALER_PCLK_DIV4;
  AdcHandle.Init.Resolution            = ADC_RESOLUTION_12B;
  AdcHandle.Init.ScanConvMode          = DISABLE;                       /* Sequencer disabled (ADC conversion on only 1 channel: channel set on rank 1) */
  AdcHandle.Init.ContinuousConvMode    = ENABLE;                        /* Continuous mode enabled to have continuous conversion  */
  AdcHandle.Init.DiscontinuousConvMode = DISABLE;                       /* Parameter discarded because sequencer is disabled */
  AdcHandle.Init.NbrOfDiscConversion   = 0;
  AdcHandle.Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_NONE;        /* Conversion start trigged at each external event */
  AdcHandle.Init.ExternalTrigConv      = ADC_EXTERNALTRIGCONV_T1_CC1;
  AdcHandle.Init.DataAlign             = ADC_DATAALIGN_RIGHT;
  AdcHandle.Init.NbrOfConversion       = 1;
  AdcHandle.Init.DMAContinuousRequests = ENABLE;
  AdcHandle.Init.EOCSelection          = DISABLE;

  if (HAL_ADC_Init(&AdcHandle) != HAL_OK) return status_error;

  /*##-2- Configure ADC regular channel ######################################*/
  sConfig.Channel      = BAT_ADC_CHANNEL;
  sConfig.Rank         = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  sConfig.Offset       = 0;

  if (HAL_ADC_ConfigChannel(&AdcHandle, &sConfig) != HAL_OK) return status_error;

  /*##-3- Start the conversion process #######################################*/
  if(HAL_ADC_Start_DMA(&AdcHandle, (uint32_t*)&uhADCxConvertedValue, 1) != HAL_OK) return status_error;

  return status_ok;
}

uint16_t voltage_get_measurement(void)
{
  return uhADCxConvertedValue;
}

void bat_adc_msp_init(ADC_HandleTypeDef *hadc)
{
  GPIO_InitTypeDef          GPIO_InitStruct;

  /*##-1- Enable peripherals and GPIO Clocks #################################*/
  /* ADC1 Periph clock enable */
  BAT_ADC_CLK_ENABLE();
  /* Enable GPIO clock ****************************************/
  BAT_ADC_CHANNEL_GPIO_CLK_ENABLE();
  /* Enable DMA2 clock */
  BAT_ADC_DMA_CLK_ENABLE();

  /*##-2- Configure peripheral GPIO ##########################################*/
  /* ADC Channel GPIO pin configuration */
  GPIO_InitStruct.Pin = BAT_ADC_CHANNEL_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(BAT_ADC_CHANNEL_GPIO_PORT, &GPIO_InitStruct);

  /*##-3- Configure the DMA streams ##########################################*/
  /* Set the parameters to be configured */
  hdma_adc.Instance = BAT_ADC_DMA_STREAM;

  hdma_adc.Init.Channel  = BAT_ADC_DMA_CHANNEL;
  hdma_adc.Init.Direction = DMA_PERIPH_TO_MEMORY;
  hdma_adc.Init.PeriphInc = DMA_PINC_DISABLE;
  hdma_adc.Init.MemInc = DMA_MINC_ENABLE;
  hdma_adc.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
  hdma_adc.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
  hdma_adc.Init.Mode = DMA_CIRCULAR;
  hdma_adc.Init.Priority = DMA_PRIORITY_MEDIUM;
  hdma_adc.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
  hdma_adc.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_HALFFULL;
  hdma_adc.Init.MemBurst = DMA_MBURST_SINGLE;
  hdma_adc.Init.PeriphBurst = DMA_PBURST_SINGLE;

  HAL_DMA_Init(&hdma_adc);

  /* Associate the initialized DMA handle to the ADC handle */
  __HAL_LINKDMA(hadc, DMA_Handle, hdma_adc);
}

void bat_adc_msp_deinit(ADC_HandleTypeDef *hadc)
{
  /*##-1- Reset peripherals ##################################################*/
  BAT_ADC_FORCE_RESET();
  BAT_ADC_RELEASE_RESET();

  /*##-2- Disable peripherals and GPIO Clocks ################################*/
  /* De-initialize the ADC Channel GPIO pin */
  HAL_GPIO_DeInit(BAT_ADC_CHANNEL_GPIO_PORT, BAT_ADC_CHANNEL_PIN);
}
