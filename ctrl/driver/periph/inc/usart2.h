#ifndef __USART2_H
#define __USART2_H

#include "periph.h"

status_t usart2_init(uint32_t baudrate);
status_t usart2_deinit(void);
status_t usart2_tx_bytes(uint8_t *p, uint32_t l);
status_t usart2_tx_bytes_it(uint8_t *p, uint32_t l);
status_t usart2_tx_bytes_dma(uint8_t *p, uint32_t l);

uint32_t usart2_rx_bytes(uint8_t *p, uint32_t l);
uint32_t usart2_cache_usage(void);

#endif /* __USART2_H */
