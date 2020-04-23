/*
 * steerif.c
 *
 *  Created on: Apr 22, 2020
 *      Author: kychu
 */

#include "steerif.h"

status_t steerif_init(void)
{
  /* Timer handler declaration */
  TIM_HandleTypeDef    TimHandle;
  /* Timer Output Compare Configuration Structure declaration */
  TIM_OC_InitTypeDef sConfig;

  /*##-1- Configure the TIM peripheral #######################################*/
  /* -----------------------------------------------------------------------
  TIM8 Configuration: generate 1 PWM signals with 0% duty cycles.

    In this driver TIM8 input clock (TIM8CLK) is set to APB2 clock x 2,
    since APB2 prescaler is equal to 2.
      TIM8CLK = APB2CLK*2
      APB2CLK = HCLK/2
      => TIM8CLK = HCLK = SystemCoreClock

    To get TIM8 counter clock at 1 MHz, the prescaler is computed as follows:
       Prescaler = (TIM8CLK / TIM8 counter clock) - 1
       Prescaler = ((SystemCoreClock) /1 MHz) - 1

    To get TIM8 output clock at 50 Hz, the period (ARR)) is computed as follows:
       ARR = (TIM8 counter clock / TIM8 output clock) - 1
           = 19999

    TIM8 Channel4 duty cycle = (TIM8_CCR4/ TIM8_ARR + 1)* 100

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
  TimHandle.Instance = STEER_TIM;
  TimHandle.State = HAL_TIM_STATE_RESET;

  TimHandle.Init.Prescaler         = 215;
  TimHandle.Init.Period            = 19999;
  TimHandle.Init.ClockDivision     = 0;
  TimHandle.Init.CounterMode       = TIM_COUNTERMODE_UP;
  TimHandle.Init.RepetitionCounter = 0;
  TimHandle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&TimHandle) != HAL_OK) return status_error; /* Initialization Error */

  /*##-2- Configure the PWM channels #########################################*/
  /* Common configuration for channel4 */
  sConfig.OCMode       = TIM_OCMODE_PWM1;
  sConfig.OCPolarity   = TIM_OCPOLARITY_HIGH;
  sConfig.OCFastMode   = TIM_OCFAST_DISABLE;
  sConfig.OCNPolarity  = TIM_OCNPOLARITY_HIGH;
  sConfig.OCNIdleState = TIM_OCNIDLESTATE_RESET;

  sConfig.OCIdleState  = TIM_OCIDLESTATE_RESET;

  /* Set the pulse value for channel 4 */
  sConfig.Pulse = 1500;
  if (HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, STEER_TIM_CHANNEL) != HAL_OK) return status_error; /* Initialization Error */

  /*##-3- Start PWM signals generation #######################################*/
  /* Start channel 4 */
  if (HAL_TIM_PWM_Start(&TimHandle, STEER_TIM_CHANNEL) != HAL_OK) return status_error; /* Initialization Error */

  return status_ok;
}

status_t steerif_set_angle(uint32_t ang)
{
  if(ang < 1000) {
    STEER_TIM->CCR4 = ang + 1000;
    return status_ok;
  }
  return status_error;
}
#include "log.h"
void steerif_msp_init(TIM_HandleTypeDef *htim)
{
  GPIO_InitTypeDef   GPIO_InitStruct;
  /*##-1- Enable peripherals and GPIO Clocks #################################*/
  /* TIM8 Peripheral clock enable */
  STEER_TIM_CLK_ENABLE();

  /* Enable all GPIO Channels Clock requested */
  STEER_TIM_CHANNEL_GPIO_PORT();
ky_info("TIM8", "msp init ...");
  /* Configure PB.03 in output, push-pull, alternate function mode */
  /* Common configuration for all channels */
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

  GPIO_InitStruct.Alternate = STEER_TIM_GPIO_AF_CHANNEL4;
  GPIO_InitStruct.Pin = STEER_TIM_GPIO_PIN_CHANNEL4;
  HAL_GPIO_Init(STEER_TIM_GPIO_PORT_CHANNEL4, &GPIO_InitStruct);
}
