/**
  ******************************************************************************
  * @file    handlers.s
  * @author  kyChu
  * @version V1.0.0
  * @date    5-March-2020
  * @brief   simple handler for hard-fault
  ******************************************************************************
  */

  .text
  .syntax unified
  .cpu cortex-m7
  .fpu fpv5-sp-d16
  .thumb

  .type HardFault_Handler, %function
  .global HardFault_Handler
  .global hard_fault_handler_c

HardFault_Handler:
  MOVS    r0, #4
  MOV     r1, lr
  TST     r0, r1
  BEQ     hf_stacking_used_MSP
  MRS     r0, psp /* first argument - PSP */
  LDR     r1, = hard_fault_handler_c
  BX      r1
hf_stacking_used_MSP:
  MRS     r0, msp /* first argument - MSP */
  LDR     r1, = hard_fault_handler_c
  BX      r1
  .end

/******************** kyChu<kyChu@qq.com> **** END OF FILE ********************/
