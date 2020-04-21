/*
 * buttons.c
 *
 *  Created on: Apr 15, 2020
 *      Author: kychu
 */

#include "buttons.h"

typedef struct {
  GPIO_TypeDef* GPIOx;
  uint16_t GPIO_Pin;
  GPIO_PinState PressState;
} USER_IO_LIST;

static const USER_IO_LIST BTN_IOS[NBTNS] = {
  {GPIOC, GPIO_PIN_0, GPIO_PIN_RESET}, // KEY PWR
  {GPIOB, GPIO_PIN_11, GPIO_PIN_RESET}, // KEY 1
  {GPIOB, GPIO_PIN_10, GPIO_PIN_RESET}, // KEY 2
  {GPIOC, GPIO_PIN_8, GPIO_PIN_RESET}, // KEY 3
};

void buttons_init(void)
{
  int i;
  GPIO_InitTypeDef  GPIO_InitStruct;

  /* Enable GPIOs clock */
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();

  /* configuration for input pins */
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  for(i = 0; i < NBTNS; i ++) {
    GPIO_InitStruct.Pin = BTN_IOS[i].GPIO_Pin;
    HAL_GPIO_Init(BTN_IOS[i].GPIOx, &GPIO_InitStruct);
  }
}

Btn_PressDef button_read(Btn_TypeDef btn)
{
  if(btn < NBTNS) {
    if(HAL_GPIO_ReadPin(BTN_IOS[btn].GPIOx, BTN_IOS[btn].GPIO_Pin) == BTN_IOS[btn].PressState)
      return btn_pressed;
    else
      return btn_released;
  }
  return btn_invalid;
}
