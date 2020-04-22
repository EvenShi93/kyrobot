/*
 * gpios.c
 *
 *  Created on: Apr 9, 2020
 *      Author: kychu
 */

#include "gpios.h"

typedef struct {
  GPIO_TypeDef* GPIOx;
  uint16_t GPIO_Pin;
} USER_IO_LIST;

static const USER_IO_LIST OUTPUT_IOS[OUTPUT_IO_NUMBER] = {
  {GPIOC, GPIO_PIN_1}, // PWR CTRL
  {GPIOC, GPIO_PIN_2}, // LED BLUE
  {GPIOC, GPIO_PIN_3}, // LED GREEN
  {GPIOC, GPIO_PIN_13}, // OTG FS SWT
  {GPIOD, GPIO_PIN_2}, // DISP RST
  {GPIOA, GPIO_PIN_15} // DISP DC
};

static const USER_IO_LIST INPUT_IOS[INPUT_IO_NUMBER] = {
  {GPIOB, GPIO_PIN_10}, // KEY 2
  {GPIOB, GPIO_PIN_11}, // KEY 1
  {GPIOC, GPIO_PIN_0}, // KEY PWR
  {GPIOC, GPIO_PIN_8}, // KEY 3
};

void board_gpio_init(void)
{
  int i;
  GPIO_InitTypeDef  GPIO_InitStruct;

  /* Enable GPIOs clock */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /* configuration for output pins */
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  for(i = 0; i < OUTPUT_IO_NUMBER; i ++) {
    GPIO_InitStruct.Pin = OUTPUT_IOS[i].GPIO_Pin;
    HAL_GPIO_Init(OUTPUT_IOS[i].GPIOx, &GPIO_InitStruct);
  }

  /* configuration for input pins */
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  for(i = 0; i < INPUT_IO_NUMBER; i ++) {
    GPIO_InitStruct.Pin = INPUT_IOS[i].GPIO_Pin;
    HAL_GPIO_Init(INPUT_IOS[i].GPIOx, &GPIO_InitStruct);
  }
}

uint8_t input_port_read(uint32_t idx)
{
  uint8_t bitstatus = 0x00;
  if(INPUT_IO_NUMBER > idx) {
    if((INPUT_IOS[idx].GPIOx->IDR & INPUT_IOS[idx].GPIO_Pin) != (uint32_t)GPIO_PIN_RESET) {
      bitstatus = (uint8_t)GPIO_PIN_SET;
    } else {
      bitstatus = (uint8_t)GPIO_PIN_RESET;
    }
  }
  return bitstatus;
}

void output_port_set(uint32_t idx)
{
  if(OUTPUT_IO_NUMBER > idx) {
    OUTPUT_IOS[idx].GPIOx->BSRR = OUTPUT_IOS[idx].GPIO_Pin;
  }
}

void output_port_clear(uint32_t idx)
{
  if(OUTPUT_IO_NUMBER > idx) {
    OUTPUT_IOS[idx].GPIOx->BSRR = OUTPUT_IOS[idx].GPIO_Pin << 16;
  }
}

void output_port_toggle(uint32_t idx)
{
  if(OUTPUT_IO_NUMBER > idx) {
    OUTPUT_IOS[idx].GPIOx->ODR ^= OUTPUT_IOS[idx].GPIO_Pin;
  }
}

void output_port_write(uint32_t idx, uint32_t val)
{
  if(OUTPUT_IO_NUMBER > idx) {
    if(val != 0) {
      OUTPUT_IOS[idx].GPIOx->BSRR = OUTPUT_IOS[idx].GPIO_Pin;
    } else {
      OUTPUT_IOS[idx].GPIOx->BSRR = OUTPUT_IOS[idx].GPIO_Pin << 16;
    }
  }
}
