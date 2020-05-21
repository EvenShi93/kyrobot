/*
 * periph.c
 *
 *  Created on: May 8, 2020
 *      Author: kychu
 */

#include "periph.h"
#include <string.h>

#include "log.h"

extern const uart_dev_t usart1_dev;
extern const uart_dev_t usart2_dev;
extern const uart_dev_t uart4_dev;
extern const uart_dev_t usart6_dev;

typedef struct {
  const char *name;
  const uart_dev_t *dev;
} uart_dev_list_t;

static uart_dev_list_t const uart_dev_list[] = {
  {
    "ttyS1",
	&usart1_dev,
  },
  {
    "ttyS2",
	&usart2_dev,
  },
  {
    "ttyS4",
	&uart4_dev,
  },
  {
    "ttyS6",
	&usart6_dev,
  },
};

status_t periph_get_uart_drv(const uart_dev_t **drv, const char *name)
{
  for(uint32_t i = 0; i < (sizeof(uart_dev_list) / sizeof(uart_dev_list_t)); i ++) {
    if(!strcmp(name, uart_dev_list[i].name)) {
      *drv = uart_dev_list[i].dev;
      return status_ok;
    }
  }
  return status_error;
}

status_t periph_disabled(const char *tag)
{
  ky_err(tag, "PERIPH DISABLED");
  return status_error;
}
