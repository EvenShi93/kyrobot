/**
  ******************************************************************************
  * @file    bsp/leds.c
  * @author  kyChu<kychu@qq.com>
  * @brief   LED driver.
  ******************************************************************************
  */

#include "leds.h"

typedef struct {
  GPIO_TypeDef* GPIOx;
  uint16_t GPIO_Pin;
  GPIO_PinState onState;
} USER_IO_LIST;

static const USER_IO_LIST LED_IOS[NLEDS] = {
  {GPIOC, GPIO_PIN_2, GPIO_PIN_SET}, // LED BLUE
  {GPIOC, GPIO_PIN_3, GPIO_PIN_SET}, // LED GREEN
};

void leds_init(void)
{
  int i;
  GPIO_InitTypeDef  GPIO_InitStruct;
  /* Enable GPIOs clock */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  /* configuration for output pins */
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  for(i = 0; i < NLEDS; i ++) {
    GPIO_InitStruct.Pin = LED_IOS[i].GPIO_Pin;
    HAL_GPIO_Init(LED_IOS[i].GPIOx, &GPIO_InitStruct);
  }
}

void led_on(Led_TypeDef led)
{
  if(led < NLEDS) {
    if(LED_IOS[led].onState == GPIO_PIN_SET)
      LED_IOS[led].GPIOx->BSRR = LED_IOS[led].GPIO_Pin;
    else
      LED_IOS[led].GPIOx->BSRR = LED_IOS[led].GPIO_Pin << 16;
  }
}

void led_off(Led_TypeDef led)
{
  if(led < NLEDS) {
    if(LED_IOS[led].onState == GPIO_PIN_SET)
      LED_IOS[led].GPIOx->BSRR = LED_IOS[led].GPIO_Pin << 16;
    else
      LED_IOS[led].GPIOx->BSRR = LED_IOS[led].GPIO_Pin;
  }
}

void led_toggle(Led_TypeDef led)
{
  if(led < NLEDS)
    LED_IOS[led].GPIOx->ODR ^= LED_IOS[led].GPIO_Pin;
}

/******************** kyChu<kyChu@qq.com> **** END OF FILE ********************/
