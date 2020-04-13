/*
 * gpios.h
 *
 *  Created on: Apr 9, 2020
 *      Author: kychu
 */

#ifndef BSP_INC_GPIOS_H_
#define BSP_INC_GPIOS_H_

#include "SysConfig.h"

#define INPUT_IO_NUMBER        4
#define OUTPUT_IO_NUMBER       7

typedef enum {
  IO_BTN2 = 0,
  IO_BTN1 = 1,
  IO_PWR_STA = 2,
  IO_BTN3 = 3,
} INPUT_IO_ID;

typedef enum {
  IO_PWR_CTRL = 0,
  IO_LED_BLUE = 1,
  IO_LED_GREEN = 2,
  IO_OTG_SWT = 3,
  IO_DISP_RST = 4,
  IO_DISP_BL = 5,
  IO_DISP_DC = 6
} OUTPUT_IO_ID;

void board_gpio_init(void);
uint8_t input_port_read(uint32_t idx);

void output_port_set(uint32_t idx);
void output_port_clear(uint32_t idx);
void output_port_toggle(uint32_t idx);
void output_port_write(uint32_t idx, uint32_t val);

#endif /* BSP_INC_GPIOS_H_ */
