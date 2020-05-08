/*
 * periph.c
 *
 *  Created on: May 8, 2020
 *      Author: kychu
 */

#include "periph.h"

#include "log.h"

status_t periph_disabled(const char *tag)
{
  ky_err(tag, "PERIPH DISABLED");
  return status_error;
}
