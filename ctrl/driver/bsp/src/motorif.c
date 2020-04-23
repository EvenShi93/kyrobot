/*
 * motorif.c
 *
 *  Created on: Apr 23, 2020
 *      Author: kychu
 */

#include "motorif.h"

status_t motorif_init(void)
{
  /* Timer handler declaration */
  TIM_HandleTypeDef    TimHandle;
  /* Timer Output Compare Configuration Structure declaration */
  TIM_OC_InitTypeDef sConfig;

  /*##-1- Configure the TIM peripheral #######################################*/
  /* -----------------------------------------------------------------------
  TIM1 Configuration: generate 2 PWM signals with 0% duty cycles.

    In this driver TIM1 input clock (TIM1CLK) is set to APB2 clock x 2,
    since APB2 prescaler is equal to 2.
      TIM1CLK = APB2CLK*2
      APB2CLK = HCLK/2
      => TIM1CLK = HCLK = SystemCoreClock

    To get TIM1 counter clock at 12 MHz, the prescaler is computed as follows:
       Prescaler = (TIM1CLK / TIM1 counter clock) - 1
       Prescaler = ((SystemCoreClock) /12 MHz) - 1

    To get TIM1 output clock at 12 KHz, the period (ARR)) is computed as follows:
       ARR = (TIM1 counter clock / TIM1 output clock) - 1
           = 999

    TIM1 Channel1 duty cycle = (TIM1_CCR1/ TIM1_ARR + 1)* 100
    TIM1 Channel2 duty cycle = (TIM1_CCR2/ TIM1_ARR + 1)* 100

    Note:
     SystemCoreClock variable holds HCLK frequency and is defined in system_stm32f7xx.c file.
     Each time the core clock (HCLK) changes, user had to update SystemCoreClock
     variable value. Otherwise, any configuration based on this variable will be incorrect.
     This variable is updated in three ways:
      1) by calling CMSIS function SystemCoreClockUpdate()
      2) by calling HAL API function HAL_RCC_GetSysClockFreq()
      3) each time HAL_RCC_ClockConfig() is called to configure the system clock frequency
  ----------------------------------------------------------------------- */

  /* Initialize TIM8 peripheral as follows:
       + Prescaler = ((SystemCoreClock) / 1000000) - 1
       + Period = (20000 - 1)
       + ClockDivision = 0
       + Counter direction = Up
  */
  TimHandle.Instance = MOTOR_TIM;
  TimHandle.State = HAL_TIM_STATE_RESET;

  TimHandle.Init.Prescaler         = 17;
  TimHandle.Init.Period            = 999;
  TimHandle.Init.ClockDivision     = 0;
  TimHandle.Init.CounterMode       = TIM_COUNTERMODE_UP;
  TimHandle.Init.RepetitionCounter = 0;
  TimHandle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&TimHandle) != HAL_OK) return status_error; /* Initialization Error */

  /*##-2- Configure the PWM channels #########################################*/
  /* Common configuration for channels */
  sConfig.OCMode       = TIM_OCMODE_PWM1;
  sConfig.OCPolarity   = TIM_OCPOLARITY_HIGH;
  sConfig.OCFastMode   = TIM_OCFAST_DISABLE;
  sConfig.OCNPolarity  = TIM_OCNPOLARITY_HIGH;
  sConfig.OCNIdleState = TIM_OCNIDLESTATE_RESET;

  sConfig.OCIdleState  = TIM_OCIDLESTATE_RESET;

  /* Set the pulse value for channel 1 */
  sConfig.Pulse = 0;
  if (HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, MOTOR_TIM_CHANNEL_A) != HAL_OK) return status_error; /* Initialization Error */
  /* Set the pulse value for channel 2 */
  sConfig.Pulse = 0;
  if (HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, MOTOR_TIM_CHANNEL_B) != HAL_OK) return status_error; /* Initialization Error */

  /*##-3- Start PWM signals generation #######################################*/
  /* Start channel 1 */
  if (HAL_TIM_PWM_Start(&TimHandle, MOTOR_TIM_CHANNEL_A) != HAL_OK) return status_error; /* Initialization Error */
  /* Start channel 2 */
  if (HAL_TIM_PWM_Start(&TimHandle, MOTOR_TIM_CHANNEL_B) != HAL_OK) return status_error; /* Initialization Error */

  return status_ok;
}

status_t motorif_set_dutycycle_a(uint32_t dutycycle)
{
  if(dutycycle <= 1000) {
    MOTOR_TIM->CCR1 = dutycycle;
    return status_ok;
  }
  return status_error;
}

status_t motorif_set_dutycycle_b(uint32_t dutycycle)
{
  if(dutycycle <= 1000) {
    MOTOR_TIM->CCR2 = dutycycle;
    return status_ok;
  }
  return status_error;
}

void motorif_msp_init(TIM_HandleTypeDef *htim)
{
  GPIO_InitTypeDef   GPIO_InitStruct;
  /*##-1- Enable peripherals and GPIO Clocks #################################*/
  /* TIM1 Peripheral clock enable */
  MOTOR_TIM_CLK_ENABLE();

  /* Enable all GPIO Channels Clock requested */
  MOTOR_TIM_CHANNEL_GPIO_PORT();

  /* Configure PA.08 and PA.09 in output, push-pull, alternate function mode */
  /* Common configuration for all channels */
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = MOTOR_TIM_GPIO_AF_CHANNEL;

  GPIO_InitStruct.Pin = MOTOR_TIM_GPIO_PIN_CHANNEL_A;
  HAL_GPIO_Init(MOTOR_TIM_GPIO_PORT_CHANNEL, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = MOTOR_TIM_GPIO_PIN_CHANNEL_B;
  HAL_GPIO_Init(MOTOR_TIM_GPIO_PORT_CHANNEL, &GPIO_InitStruct);
}
