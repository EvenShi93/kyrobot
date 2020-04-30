/*
 * notifytimer.c
 *
 *  Created on: Apr 30, 2020
 *      Author: kychu
 */

#include "notifytimer.h"

static NotifyTimerCallback tim7_cb;

#define __TIM7_GET_FLAG(__FLAG__)          ((TIM7->SR &(__FLAG__)) == (__FLAG__))
#define __TIM7_GET_IT_SOURCE(__INTERRUPT__) (((TIM7->DIER & (__INTERRUPT__)) == (__INTERRUPT__)) ? SET : RESET)
#define __TIM7_CLEAR_IT(__INTERRUPT__)      (TIM7->SR = ~(__INTERRUPT__))

status_t notify_timer_init(uint32_t period_ms, NotifyTimerCallback cb)
{
  uint16_t tmpcr1 = 0;

  tim7_cb = NULL;
  if(cb == NULL) return status_error;
  tim7_cb = cb;

  /* enable TIM7 peripheral clock */
  __TIM7_CLK_ENABLE();

  tmpcr1 = TIM7->CR1;
  /* Set the auto-reload preload */
  MODIFY_REG(tmpcr1, TIM_CR1_ARPE, TIM_AUTORELOAD_PRELOAD_ENABLE);
  TIM7->CR1 = tmpcr1;
  TIM7->ARR = period_ms * 10 - 1; /* period in millisecond */
  TIM7->PSC = 10799; /* 100us */
  /* Generate an update event to reload the Prescaler
     and the repetition counter (only for advanced timer) value immediately */
  TIM7->EGR = TIM_EGR_UG;

  TIM7->CNT = 0;
  TIM7->DIER |= TIM_IT_UPDATE;
  TIM7->CR1 |= TIM_CR1_CEN;

  /* NVIC for TIM7, to catch the update event */
  HAL_NVIC_SetPriority(TIM7_IRQn, INT_PRIO_NOTIFY_TIMER, 0);
  HAL_NVIC_EnableIRQ(TIM7_IRQn);

  return status_ok;
}

void TIM7_IRQHandler(void)
{
  if (__TIM7_GET_FLAG(TIM_FLAG_UPDATE) != RESET) {
    if(__TIM7_GET_IT_SOURCE(TIM_IT_UPDATE) != RESET) {
      __TIM7_CLEAR_IT(TIM_IT_UPDATE);
      if(tim7_cb != NULL) {
        tim7_cb();
      }
    }
  }
}
