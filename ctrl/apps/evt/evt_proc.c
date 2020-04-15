/*
 * evt_proc.c
 *
 *  Created on: Apr 14, 2020
 *      Author: kychu
 */

#include "apps.h"

struct BtnState {
  uint32_t state;
} btn_state[4];

void evt_proc_task(void const *argument)
{
  buttons_init();
  for(;;) {
    button_read(BTN0);
    delay(1000);
  }
}
