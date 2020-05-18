#ifndef __SYS_DEF_H
#define __SYS_DEF_H

#include "stddef.h"
#include "stdbool.h"

#ifndef NULL
 #define NULL    0
#endif

#ifndef true
 #define true 1
#endif

#ifndef false
 #define false 0
#endif

/* __packed keyword used to decrease the data type alignment to 1-byte */
#if defined (__CC_ARM)         /* ARM Compiler */
  #define __PACK_BEGIN  __packed
  #define __PACK_END
#elif defined (__ICCARM__)     /* IAR Compiler */
  #define __PACK_BEGIN  __packed
  #define __PACK_END
#elif defined   ( __GNUC__ )   /* GNU Compiler */
  #define __PACK_BEGIN
  #define __PACK_END    __attribute__ ((__packed__))
#elif defined   (__TASKING__)  /* TASKING Compiler */
  #define __PACK_BEGIN  __unaligned
  #define __PACK_END
#endif /* __CC_ARM */

#if defined   (__GNUC__)        /* GNU Compiler */
  #define __ALIGN_END    __attribute__ ((aligned (4)))
  #define __ALIGN_BEGIN
#else
  #define __ALIGN_END
  #if defined   (__CC_ARM)      /* ARM Compiler */
    #define __ALIGN_BEGIN    __align(4)
  #elif defined (__ICCARM__)    /* IAR Compiler */
    #define __ALIGN_BEGIN
  #elif defined  (__TASKING__)  /* TASKING Compiler */
    #define __ALIGN_BEGIN    __align(4)
  #endif /* __CC_ARM */
#endif /* __GNUC__ */

#if defined   (__GNUC__)        /* GNU Compiler */
  #ifndef __weak
    #define __weak   __attribute__((weak))
  #endif /* __weak */
#endif /* __GNUC__ */

typedef enum 
{
  status_ok        = 0x00U,
  status_error     = 0x01U,
  status_busy      = 0x02U,
  status_timeout   = 0x03U,
  status_nomem     = 0x04U,
  status_inv_arg   = 0x05U,
  status_not_found = 0x06U,
} status_t;

#endif /* __SYS_DEF_H */
