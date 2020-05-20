/*
 * console.c
 *
 *  Created on: May 14, 2020
 *      Author: kychu
 */

#include "console.h"

#if FREERTOS_ENABLED
#include "cmsis_os.h"

#include <ctype.h>
#include <string.h>

#include "log.h"

#define DEFAULT_CONSOLE_PROMPT              "kychu>"

enum KEY_ACTION{
	KEY_NULL = 0,	    /* NULL */
	CTRL_A = 1,         /* Ctrl+a */
	CTRL_B = 2,         /* Ctrl-b */
	CTRL_C = 3,         /* Ctrl-c */
	CTRL_D = 4,         /* Ctrl-d */
	CTRL_E = 5,         /* Ctrl-e */
	CTRL_F = 6,         /* Ctrl-f */
	CTRL_H = 8,         /* Ctrl-h */
	TAB = 9,            /* Tab */
	CTRL_K = 11,        /* Ctrl+k */
	CTRL_L = 12,        /* Ctrl+l */
	ENTER = 10,         /* Enter */
	CTRL_N = 14,        /* Ctrl-n */
	CTRL_P = 16,        /* Ctrl-p */
	CTRL_T = 20,        /* Ctrl-t */
	CTRL_U = 21,        /* Ctrl+u */
	CTRL_W = 23,        /* Ctrl+w */
	ESC = 27,           /* Escape */
	BACKSPACE =  127    /* Backspace */
};

typedef struct {
  uint8_t ln_cache[CONSOLE_LINE_CACHE_SIZE];
} console_cache_t;

static const char *TAG = "CONSOLE";

static char **argv;
static console_cfg_t *console;
static console_cache_t *console_cache;

static void console_task(void const *argument);
static void start_action(void);
static int console_proc(void);
static void sanitize(char* src);

status_t console_start(console_cfg_t *cfg)
{
  if(cfg != NULL)
    console = cfg;
  if(console->prompt == NULL) console->prompt = DEFAULT_CONSOLE_PROMPT;
  console_cache = kmm_alloc(sizeof(console_cache_t));
  if(console_cache == NULL) return status_nomem;
  memset(console_cache, 0, sizeof(console_cache_t));
  argv = (char **)kmm_alloc(CONSOLE_MAX_CMDLINE_ARGS * sizeof(char *));
  if(argv == NULL) return status_nomem;
  memset(argv, 0, CONSOLE_MAX_CMDLINE_ARGS * sizeof(char *));
  osThreadDef(CONSOLE, console_task, osPriorityNormal, 0, 256);
  if(osThreadCreate(osThread(CONSOLE), NULL) == NULL) return status_error;
  return status_ok;
}

static void console_task(void const *argument)
{
  int argc, cmd_ret;
  start_action();
  console->console_tx((uint8_t *)"\r\nkyChu Shell\r\n", 15);
  for(;;) {
    // start new command line
    console->console_tx((uint8_t *)console->prompt, strlen(console->prompt));
    if(console_proc() > 0) {
      sanitize((char *)console_cache->ln_cache);
      argc = esp_console_split_argv((char *)console_cache->ln_cache, argv, CONSOLE_MAX_CMDLINE_ARGS);
      if(console_cmd_run(argc, argv, &cmd_ret) == status_not_found) {
        ky_err(TAG, "\'%s\' command NOT found", argv[0]);
      } else {
        ky_info(TAG, "exit with (%d)", cmd_ret);
      }
    }
  }
}

static void start_action(void)
{
  // press Enter 3 times.
  uint8_t c, count = 0;
  do {
    if(console->console_rx(&c, 1) == 0) {
      delay(10);
    } else {
      if(c == '\r') {
        count ++;
      } else {
        count = 0;
      }
    }
  } while(count < 3);
}

static int console_proc(void)
{
  uint8_t c;
  uint32_t count = 0;
  while(1) {
    if(console->console_rx(&c, 1) == 0) {
      delay(10);
    } else {
      if (c == '\r') {
        console->console_tx((uint8_t *)"\r\n", 2);
        break;
      } else if (c == 0x1b) {//(c >= 0x1c && c <= 0x1f){
        continue; /* consume arrow keys */
      } else if (c == BACKSPACE || c == 0x8) {
        if (count > 0) {
          console_cache->ln_cache[count - 1] = 0;
          count --;
          console->console_tx((uint8_t *)"\x08 ", 2); /* Windows CMD: erase symbol under cursor */
        } else continue;
      } else {
        console_cache->ln_cache[count ++] = c;
        if(count == CONSOLE_LINE_CACHE_SIZE - 1) {
          console->console_tx((uint8_t *)"\r\n", 2);
          break;
        }
      }
      console->console_tx((uint8_t *)&c, 1); /* echo */
    }
  }
  console_cache->ln_cache[count] = 0;
  return count;
}

static void sanitize(char* src)
{
  char* dst = src;
  for (int c = *src; c != 0; src++, c = *src) {
    if (isprint(c)) {
      *dst = c;
      ++dst;
    }
  }
  *dst = 0;
}

#endif /* FREERTOS_ENABLED */
