/**
  ******************************************************************************
  * @file    bsp/leds.c
  * @author  kyChu<kychu@qq.com>
  * @brief   LED driver.
  ******************************************************************************
  */

#include "leds.h"

void led_on(Led_TypeDef led)
{
  if(led == LED1) {
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_RESET);
  } else {
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_RESET);
  }
}

void led_off(Led_TypeDef led)
{
  if(led == LED1) {
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_SET);
  } else {
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_SET);
  }
}

void led_toggle(Led_TypeDef led)
{
  if(led == LED1) {
    HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_2);
  } else {
    HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_3);
  }
}

/******************** kyChu<kyChu@qq.com> **** END OF FILE ********************/
