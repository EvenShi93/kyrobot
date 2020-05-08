/*
 * config.h
 *
 *  Created on: Feb 25, 2020
 *      Author: kychu
 */

#ifndef SYSCONFIG_CONFIG_H_
#define SYSCONFIG_CONFIG_H_

#define SYS_ARCH_STR                             "ROB CTL M7"

#define SYS_XCLK_FREQUENCY                       (8000000)

//#define SYS_BOOT_ORIGIN                          0x08000000
//#define SYS_BOOT_LENGTH                          32K
#define SYS_TEXT_ORIGIN                          0x08000000
#define SYS_TEXT_LENGTH                          512K
#define SYS_SRAM_ORIGIN                          0x20000000
#define SYS_SRAM_LENGTH                          256K

#define SYS_MAIN_FLASH                           FLASH_TEXT
#define SYS_MAIN_MEMORY                          SRAM

#define SYS_HEAP_SIZE                            0
#define SYS_STACK_SIZE                           512

#define FREERTOS_ENABLED                         (1)

#define CONFIG_LOG_ENABLE                        (1)
#define CONFIG_IMU_ENABLE                        (1)
#define CONFIG_USB_ENABLE                        (1)

#if FREERTOS_ENABLED
  #define SYSTICK_ENABLE                         (1)
#else
  #define SYSTICK_ENABLE                         (0)
#endif /* FREERTOS_ENABLED */

#if FREERTOS_ENABLED
#define FREERTOS_HEAP_SIZE                       (160 * 1024)
#define START_TASK_STACK_SIZE                    (256)
#endif /* FREERTOS_ENABLED */

/* Peripheral Configuration Table */
#define USART2_ENABLE                            (1)
#define USART2_TX_ENABLE                         (0)
#define USART2_RX_ENABLE                         (1)
#define USART2_DMA_TX_ENABLE                     (0)
#define USART2_DMA_RX_ENABLE                     (1)

#define USART6_ENABLE                            (1)
#define USART6_TX_ENABLE                         (1)
#define USART6_RX_ENABLE                         (0)
#define USART6_DMA_TX_ENABLE                     (0)
#define USART6_DMA_RX_ENABLE                     (0)

/* Interrupt Priority Table */
#define INT_PRIORITY_MAX                         5
#define INT_PRIORITY_MIN                         15

#define INT_PRIORITY_LIMIT_MAX(prio)             ((prio) < INT_PRIORITY_MAX) ? INT_PRIORITY_MAX : (prio)
#define INT_PRIORITY_LIMIT_MIN(prio)             ((prio) > INT_PRIORITY_MIN) ? INT_PRIORITY_MIN : (prio)

#define INT_PRIORITY_LIMIT(prio)                 INT_PRIORITY_LIMIT_MIN(INT_PRIORITY_LIMIT_MAX(prio))

#define INT_PRIO_IMUIF_DMARX                     INT_PRIORITY_LIMIT(INT_PRIORITY_MAX + 0)
#define INT_PRIO_IMUIF_DMATX                     INT_PRIORITY_LIMIT(INT_PRIORITY_MAX + 0)

#define INT_PRIO_IMUIF_INT1                      INT_PRIORITY_LIMIT(INT_PRIORITY_MAX + 1)
#define INT_PRIO_IMUIF_INT2                      INT_PRIORITY_LIMIT(INT_PRIORITY_MAX + 9)

#define INT_PRIO_DISPIF_DMATX                    INT_PRIORITY_LIMIT(INT_PRIORITY_MAX + 4)

#define INT_PRIO_USART2_PERIPH                   INT_PRIORITY_LIMIT(INT_PRIORITY_MAX + 5)
#define INT_PRIO_USART2_DMARX                    INT_PRIORITY_LIMIT(INT_PRIORITY_MAX + 5)
#define INT_PRIO_USART2_DMATX                    INT_PRIORITY_LIMIT(INT_PRIORITY_MAX + 5)

#define INT_PRIO_NOTIFY_TIMER                    INT_PRIORITY_LIMIT(INT_PRIORITY_MAX + 6)

#define INT_PRIO_GSMIF_PERIPH                    INT_PRIORITY_LIMIT(INT_PRIORITY_MAX + 4)
#define INT_PRIO_GSMIF_DMARX                     INT_PRIORITY_LIMIT(INT_PRIORITY_MAX + 4)
#define INT_PRIO_GSMIF_DMATX                     INT_PRIORITY_LIMIT(INT_PRIORITY_MAX + 4)

#define INT_PRIO_UBXAIF_PERIPH                   INT_PRIORITY_LIMIT(INT_PRIORITY_MAX + 6)
#define INT_PRIO_UBXAIF_DMARX                    INT_PRIORITY_LIMIT(INT_PRIORITY_MAX + 6)
#define INT_PRIO_UBXAIF_DMATX                    INT_PRIORITY_LIMIT(INT_PRIORITY_MAX + 6)

#define INT_PRIO_MAGIF_DMARX                     INT_PRIORITY_LIMIT(INT_PRIORITY_MAX + 8)
#define INT_PRIO_MAGIF_DMATX                     INT_PRIORITY_LIMIT(INT_PRIORITY_MAX + 8)
#define INT_PRIO_MAGIF_ER                        INT_PRIORITY_LIMIT(INT_PRIORITY_MAX + 9)
#define INT_PRIO_MAGIF_EV                        INT_PRIORITY_LIMIT(INT_PRIORITY_MAX + 9)

#define INT_PRIO_USART6_PERIPH                   INT_PRIORITY_LIMIT(INT_PRIORITY_MAX + 10)
#define INT_PRIO_USART6_DMARX                    INT_PRIORITY_LIMIT(INT_PRIORITY_MAX + 10)
#define INT_PRIO_USART6_DMATX                    INT_PRIORITY_LIMIT(INT_PRIORITY_MAX + 10)

#define INT_PRIO_OTGFS_INT                       INT_PRIORITY_LIMIT(INT_PRIORITY_MAX + 9)

#define INT_PRIO_FLASHIF_IT                      INT_PRIORITY_LIMIT(INT_PRIORITY_MAX + 2)

#define INT_PRIO_RTCMIF_PERIPH                   INT_PRIORITY_LIMIT(INT_PRIORITY_MAX + 7)
#define INT_PRIO_RTCMIF_DMARX                    INT_PRIORITY_LIMIT(INT_PRIORITY_MAX + 5)
#define INT_PRIO_RTCMIF_DMATX                    INT_PRIORITY_LIMIT(INT_PRIORITY_MAX + 7)

#if CONFIG_USB_ENABLE
/*  Total size of IN buffer:  Total size of USB IN buffer: APP_RX_DATA_SIZE*8/MAX_BAUDARATE*1000 should be > CDC_IN_FRAME_INTERVAL */
#define APP_RX_DATA_SIZE                         256
#endif /* CONFIG_USB_IF_ENABLE */

#define USART2_USER_RX_CACHE_DEPTH               (256)
#define USART6_USER_RX_CACHE_DEPTH               (256)

#define CONFIG_STEMWIN_DEMO_ENABLE               (0)

#endif /* SYSCONFIG_CONFIG_H_ */

/******************** kyChu<kyChu@qq.com> **** END OF FILE ********************/
