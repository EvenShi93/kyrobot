/*
 * console.h
 *
 *  Created on: May 14, 2020
 *      Author: kychu
 */

#ifndef CONSOLE_CONSOLE_H_
#define CONSOLE_CONSOLE_H_

#include "shell.h"

#define CONSOLE_TASK_STACK_SIZE        1024
#define CONSOLE_LINE_CACHE_SIZE        128
#define CONSOLE_MAX_CMDLINE_ARGS       8

typedef struct {
  const char *prompt;
  uint32_t (* console_rx)(uint8_t *p, uint32_t l);
  status_t (* console_tx)(uint8_t *p, uint32_t l);
} console_cfg_t;

status_t console_start(console_cfg_t *console);

#endif /* CONSOLE_CONSOLE_H_ */
