/*
 * printHardFault.c
 *
 *  Created on: Mar 8, 2019
 *      Author: kychu
 */

#include "log.h"

static const char *TAG = "HARDFAULT";

/* Private functions ---------------------------------------------------------*/
void hard_fault_handler_c(unsigned int * hf_args)
{
  /* R0   = hf_args[0]
     R1   = hf_args[1]
     R2   = hf_args[2]
     R3   = hf_args[3]
     R12  = hf_args[4]
     LR   = hf_args[5]
     PC   = hf_args[6]
     xPSR = hf_args[7]
  */
  ky_err(TAG, " - R0 = 0x%08x", hf_args[0]);
  ky_err(TAG, " - R1 = 0x%08x", hf_args[1]);
  ky_err(TAG, " - R2 = 0x%08x", hf_args[2]);
  ky_err(TAG, " - R3 = 0x%08x", hf_args[3]);
  ky_err(TAG, " - R12 = 0x%08x", hf_args[4]);
  ky_err(TAG, " - LR = 0x%08x", hf_args[5]);
  ky_err(TAG, " - PC = 0x%08x", hf_args[6]);
  ky_err(TAG, " - xPSR = 0x%08x", hf_args[7]);
  for(;;);
}
