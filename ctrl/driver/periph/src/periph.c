/*
 * periph.c
 *
 *  Created on: May 8, 2020
 *      Author: kychu
 */

#include "periph.h"
#include <string.h>

#include "usart1.h"
#include "usart2.h"
#include "uart4.h"
#include "usart6.h"

#include "log.h"

static const uart_dev_t usart1_dev = {
  "ttyS1",
  usart1_init,
  usart1_deinit,
  usart1_rx_bytes,
  usart1_tx_bytes,
  usart1_tx_bytes_it,
  usart1_tx_bytes_dma,
  usart1_cache_usage,
};

static const uart_dev_t usart2_dev = {
  "ttyS2",
  usart2_init,
  usart2_deinit,
  usart2_rx_bytes,
  usart2_tx_bytes,
  usart2_tx_bytes_it,
  usart2_tx_bytes_dma,
  usart2_cache_usage,
};

static const uart_dev_t uart4_dev = {
  "ttyS4",
  uart4_init,
  uart4_deinit,
  uart4_rx_bytes,
  uart4_tx_bytes,
  uart4_tx_bytes_it,
  uart4_tx_bytes_dma,
  uart4_cache_usage,
};

static const uart_dev_t usart6_dev = {
  "ttyS6",
  usart6_init,
  usart6_deinit,
  usart6_rx_bytes,
  usart6_tx_bytes,
  usart6_tx_bytes_it,
  usart6_tx_bytes_dma,
  usart6_cache_usage,
};

static const uart_dev_t *uart_dev_list[] = {
  &usart1_dev,
  &usart2_dev,
  &uart4_dev,
  &usart6_dev,
};

status_t periph_get_uart_drv(const uart_dev_t *drv, const char *name)
{
  for(uint32_t i = 0; i < (sizeof(uart_dev_list) / sizeof(uart_dev_t *)); i ++) {
    if(strcmp(name, uart_dev_list[i]->uart_name) == 0) {
      drv = uart_dev_list[i];
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
