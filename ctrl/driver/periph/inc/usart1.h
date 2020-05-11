#ifndef __USART1_H
#define __USART1_H

#include "periph.h"

status_t usart1_init(uint32_t baudrate);
status_t usart1_deinit(void);
status_t usart1_tx_bytes(uint8_t *p, uint32_t l);
status_t usart1_tx_bytes_it(uint8_t *p, uint32_t l);
status_t usart1_tx_bytes_dma(uint8_t *p, uint32_t l);

uint32_t usart1_rx_bytes(uint8_t *p, uint32_t l);
uint32_t usart1_cache_usage(void);

#endif /* __USART1_H */
