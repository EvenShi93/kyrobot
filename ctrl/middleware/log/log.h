/*
 * log.h
 *
 *  Created on: Dec 10, 2019
 *      Author: kychu
 */

#ifndef _LOG_H_
#define _LOG_H_

#include "logconfig.h"

/**
 * @brief Log level
 */
typedef enum {
  LOG_NONE,       /*!< No log output */
  LOG_ERROR,      /*!< Critical errors, software module can not recover on its own */
  LOG_WARN,       /*!< Error conditions from which recovery measures have been taken */
  LOG_INFO,       /*!< Information messages which describe normal flow of events */
  LOG_DEBUG,      /*!< Extra information which is not necessary for normal use (values, pointers, sizes, etc). */
  LOG_VERBOSE     /*!< Bigger chunks of debugging information, or frequent messages which can potentially flood the output. */
} log_level_t;

typedef status_t (*log_put_t)(const char *);

#if CONFIG_LOG_ENABLE
status_t log_init(log_put_t ptx);
status_t log_write(const char *format, ...) __attribute__ ((__format__ (__printf__, 1, 2)));
#else
#define log_init(ptx)                            do { (void)ptx; } while(0)
#define log_write(format, ...)                   do { (void)format; } while(0)
#endif /* CONFIG_LOG_ENABLE */

uint32_t log_timestamp(void);

#if CONFIG_LOG_COLORS_ENABLED
#define LOG_COLOR_BLACK   "30"
#define LOG_COLOR_RED     "31"
#define LOG_COLOR_GREEN   "32"
#define LOG_COLOR_BROWN   "33"
#define LOG_COLOR_BLUE    "34"
#define LOG_COLOR_PURPLE  "35"
#define LOG_COLOR_CYAN    "36"
#define LOG_COLOR(COLOR)  "\033[0;" COLOR "m"
#define LOG_BOLD(COLOR)   "\033[1;" COLOR "m"
#define LOG_RESET_COLOR   "\033[0m"
#define LOG_COLOR_E       LOG_COLOR(LOG_COLOR_RED)
#define LOG_COLOR_W       LOG_COLOR(LOG_COLOR_BROWN)
#define LOG_COLOR_I       LOG_COLOR(LOG_COLOR_GREEN)
#define LOG_COLOR_D
#define LOG_COLOR_V
#else //CONFIG_LOG_COLORS_ENABLED
#define LOG_COLOR_E
#define LOG_COLOR_W
#define LOG_COLOR_I
#define LOG_COLOR_D
#define LOG_COLOR_V
#define LOG_RESET_COLOR
#endif //CONFIG_LOG_COLORS_ENABLED

#define LOG_FORMAT(letter, format)  LOG_COLOR_ ## letter #letter " (%ld) %s: " format LOG_RESET_COLOR "\n"

#define LOG_LEVEL(level, tag, format, ...) do {                         \
        if (level==LOG_ERROR )          { log_write(LOG_FORMAT(E, format), log_timestamp(), tag, ##__VA_ARGS__); } \
        else if (level==LOG_WARN )      { log_write(LOG_FORMAT(W, format), log_timestamp(), tag, ##__VA_ARGS__); } \
        else if (level==LOG_DEBUG )     { log_write(LOG_FORMAT(D, format), log_timestamp(), tag, ##__VA_ARGS__); } \
        else if (level==LOG_VERBOSE )   { log_write(LOG_FORMAT(V, format), log_timestamp(), tag, ##__VA_ARGS__); } \
        else                            { log_write(LOG_FORMAT(I, format), log_timestamp(), tag, ##__VA_ARGS__); } \
    } while(0)

#if CONFIG_LOG_ERROR_ENABLED
#define ky_err(tag, format, ...)               log_write(LOG_FORMAT(E, format), log_timestamp(), tag, ##__VA_ARGS__)
#else
#define ky_err(tag, format, ...)               do { (void)tag; } while(0)
#endif /* CONFIG_LOG_ERROR_ENABLED */
#if CONFIG_LOG_WARN_ENABLED
#define ky_warn(tag, format, ...)              log_write(LOG_FORMAT(W, format), log_timestamp(), tag, ##__VA_ARGS__)
#else
#define ky_warn(tag, format, ...)              do { (void)tag; } while(0)
#endif /* CONFIG_LOG_WARN_ENABLED */
#if CONFIG_LOG_INFO_ENABLED
#define ky_info(tag, format, ...)              log_write(LOG_FORMAT(I, format), log_timestamp(), tag, ##__VA_ARGS__)
#else
#define ky_info(tag, format, ...)              do { (void)tag; } while(0)
#endif /* CONFIG_LOG_INFO_ENABLED */

void log_buffer_hex(const char *tag, const void *buffer, uint16_t buff_len, log_level_t log_level);
void log_buffer_char(const char *tag, const void *buffer, uint16_t buff_len, log_level_t log_level);
void log_buffer_hexdump(const char *tag, const void *buffer, uint16_t buff_len, log_level_t log_level);

#endif /* _LOG_H_ */
