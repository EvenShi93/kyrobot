/*
 * encoderif.c
 *
 *  Created on: Apr 30, 2020
 *      Author: kychu
 */

#include "encoderif.h"

status_t encoderif_init(void)
{
  /* Timer handler declaration */
  TIM_HandleTypeDef    TimHandle;
  TIM_Encoder_InitTypeDef sConfig;

  TimHandle.Instance = ENC_TIM;
  TimHandle.State = HAL_TIM_STATE_RESET;

  TimHandle.Init.Prescaler         = 0; // ((216MHz / 4) * 2) = 108MHz
  TimHandle.Init.Period            = 0xFFFF;
  TimHandle.Init.ClockDivision     = 0;
  TimHandle.Init.CounterMode       = TIM_COUNTERMODE_UP;
  TimHandle.Init.RepetitionCounter = 0;
  TimHandle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

  sConfig.EncoderMode = TIM_ENCODERMODE_TI12;
  sConfig.IC1Filter = 0;
  sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC2Filter = 0;
  sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;

  if(HAL_TIM_Encoder_Init(&TimHandle, &sConfig) != HAL_OK) return status_error;

  if(HAL_TIM_Encoder_Start(&TimHandle, TIM_CHANNEL_ALL) != HAL_OK) return status_error;
  __HAL_TIM_SET_COUNTER(&TimHandle, 0);
  return status_ok;
}

int16_t encoderif_take_counter(void)
{
  int16_t ret;
  ret = ENC_TIM->CNT;
  ENC_TIM->CNT = 0;
  return ret;
}

void encoderif_msp_init(TIM_HandleTypeDef *htim)
{
  GPIO_InitTypeDef   GPIO_InitStruct;
  /*##-1- Enable peripherals and GPIO Clocks #################################*/
  /* TIM3 Peripheral clock enable */
  ENC_TIM_CLK_ENABLE();

  /* Enable all GPIO Channels Clock requested */
  ENC_TIM_CHANNEL_GPIO_PORT();

  /* Configure PB.04, PB.05 in output, push-pull, alternate function mode */
  /* Common configuration for all channels */
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = ENC_TIM_GPIO_AF_CHANNEL;

  GPIO_InitStruct.Pin = ENC_TIM_GPIO_PIN_CHANNEL1;
  HAL_GPIO_Init(ENC_TIM_GPIO_PORT_CHANNEL, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = ENC_TIM_GPIO_PIN_CHANNEL2;
  HAL_GPIO_Init(ENC_TIM_GPIO_PORT_CHANNEL, &GPIO_InitStruct);
}
