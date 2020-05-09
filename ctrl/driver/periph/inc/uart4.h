#ifndef __UART4_H
#define __UART4_H

#include "periph.h"

status_t uart4_init(uint32_t baudrate);
status_t uart4_deinit(void);
status_t uart4_tx_bytes(uint8_t *p, uint32_t l);
status_t uart4_tx_bytes_it(uint8_t *p, uint32_t l);
status_t uart4_tx_bytes_dma(uint8_t *p, uint32_t l);

uint32_t uart4_rx_bytes(uint8_t *p, uint32_t l);
uint32_t uart4_cache_usage(void);

#endif /* __UART4_H */
