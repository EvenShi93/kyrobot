/*
 * periph.h
 *
 *  Created on: May 8, 2020
 *      Author: kychu
 */

#ifndef PERIPH_INC_PERIPH_H_
#define PERIPH_INC_PERIPH_H_

#include "SysConfig.h"

#include "periph_config.h"

typedef struct {
  const char *uart_name;
  status_t (* uart_init)(uint32_t baudrate);
  status_t (* uart_deinit)(void);
  uint32_t (* uart_rx)(uint8_t *p, uint32_t l);
  status_t (* uart_tx)(uint8_t *p, uint32_t l);
  status_t (* uart_tx_it)(uint8_t *p, uint32_t l);
  status_t (* uart_tx_dma)(uint8_t *p, uint32_t l);
  uint32_t (* uart_cache_usage)(void);
} uart_dev_t;

status_t periph_get_uart_drv(const uart_dev_t *drv, const char *name);

status_t periph_disabled(const char *tag);

#endif /* PERIPH_INC_PERIPH_H_ */
