/*
 * buttons.h
 *
 *  Created on: Apr 15, 2020
 *      Author: kychu
 */

#ifndef BSP_INC_BUTTONS_H_
#define BSP_INC_BUTTONS_H_

#include "SysConfig.h"

#ifdef __cplusplus
 extern "C" {
#endif

#define NBTNS                  4

typedef enum {
  BTN0 = 0,
  BTN_PWR = BTN0,
  BTN1 = 1,
  BTN2 = 2,
  BTN3 = 3
} Btn_TypeDef;

typedef enum {
  btn_pressed = 0,
  btn_released = 1,
  btn_invalid = 0xF,
} Btn_PressDef;

void buttons_init(void);
Btn_PressDef button_read(Btn_TypeDef btn);

#ifdef __cplusplus
}
#endif

#endif /* BSP_INC_BUTTONS_H_ */
