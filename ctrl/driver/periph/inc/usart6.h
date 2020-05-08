#ifndef __USART6_H
#define __USART6_H

#include "periph.h"

status_t usart6_init(uint32_t baudrate);
status_t usart6_deinit(void);
status_t usart6_tx_bytes(uint8_t *p, uint32_t l);
status_t usart6_tx_bytes_it(uint8_t *p, uint32_t l);
status_t usart6_tx_bytes_dma(uint8_t *p, uint32_t l);

uint32_t usart6_rx_bytes(uint8_t *p, uint32_t l);
uint32_t usart6_cache_usage(void);

#endif /* __USART6_H */
