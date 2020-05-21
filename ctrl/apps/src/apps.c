#include "apps.h"

#include "syscmds.h"

static const char *TAG = "APP";

static const uart_dev_t *log_uart;
static console_cfg_t console_cfg;

static void led_thread(void const *argument);
static status_t log_tx_string(const char *p);

#define STR1(R) #R
#define STR2(R) STR1(R)

static const char SystemInfo[] =
"\n\r  .--,       .--,"
"\n\r ( (  \\.---./  ) )"
"\n\r  '.__/o   o\\__.'"
"\n\r     {=  ^  =}"               "\t\t\t"   "kyRobot Controller Board based on STM32F7xx"
"\n\r      >  -  <"                "\t\t\t"   "Author:  kyChu<kychu@qq.com>"
"\n\r     /       \\"              "\t\t\t"   "Version: " STR2(__VERSION_STR__)
"\n\r    //       \\\\"            "\t\t\t"   "Date:    " __DATE__
"\n\r   //|   .   |\\\\"             "\t\t"   "Time:    " __TIME__
"\n\r   \"'\\       /'\"_.-~^`'-."     "\t"   "Board:   kyRobot Core Board(20200401 V1)"
"\n\r      \\  _  /--'         `"      "\t"   "ALL RIGHTS RESERVED BY kyChu<kychu@qq.com>"
"\n\r    ___)( )(___"
"\n\r   (((__) (__)))"
"\n\r"
;

#define TEST_CASE_TASK_ENABLE                    (0)

void APP_StartThread(void const *argument)
{
  osDelay(500);
  /* Initialize GPIOs */
  board_gpio_init();

  /* lock power */
  output_port_set(IO_PWR_CTRL);

  if(periph_get_uart_drv(&log_uart, "ttyS6") != status_ok) {
    led_on(LED_BLUE);
    led_on(LED_GREEN);
    vTaskDelete(NULL);
  }
  log_uart->uart_init(115200);
  log_init(log_tx_string);
  log_tx_string("!!!KERNEL START!!!\n");
  log_tx_string(SystemInfo);

  // mount FATFS logic driver
  if(fatfs_mount() != status_ok) {
    ky_err(TAG, "ERROR: FS INIT FAIL! EXIT!");
    vTaskDelete(NULL);
  }

  syscmds_register();

  osThreadDef(UGUI, gui_task, osPriorityNormal, 0, 512);
  if(osThreadCreate(osThread(UGUI), NULL) == NULL) ky_err(TAG, "gui task create failed.");
  osThreadDef(EVET, evt_proc_task, osPriorityNormal, 0, 256);
  if(osThreadCreate(osThread(EVET), NULL) == NULL) ky_err(TAG, "event task create failed.");
//  osThreadDef(MAGS, magnetics_task, osPriorityNormal, 0, configMINIMAL_STACK_SIZE * 4); // stack size = 512B
//  if(osThreadCreate(osThread(MAGS), NULL) == NULL) ky_err(TAG, "mags task create failed.");
#if (TEST_CASE_TASK_ENABLE)
  osThreadDef(TEST, test_case_task, osPriorityNormal, 0, 1024); // stack size = 1KB
  if(osThreadCreate(osThread(TEST), NULL) == NULL) ky_err(TAG, "test task create failed.");
#endif /* (TEST_CASE_TASK_ENABLE) */
  osThreadDef(MESG, mesg_proc_task, osPriorityNormal, 0, 512);
  if(osThreadCreate(osThread(MESG), NULL) == NULL) ky_err(TAG, "mesg task create failed.");
//  osThreadDef(FILE, transfile_task, osPriorityNormal, 0, 512); // stack size = 512B
//  if(osThreadCreate(osThread(FILE), NULL) == NULL) ky_err(TAG, "file task create failed.");

  /* LED INDICATOR TASK */
  osThreadDef(LEDS, led_thread, osPriorityNormal, 0, 128); // stack size = 128B
  if(osThreadCreate(osThread(LEDS), NULL) == NULL) ky_err(TAG, "leds task create failed.");

  console_cfg.prompt = NULL;
  console_cfg.console_tx = log_uart->uart_tx;
  console_cfg.console_rx = log_uart->uart_rx;
  console_start(&console_cfg);

  vTaskDelete(NULL);
}

static void led_thread(void const *argument)
{
  (void) argument;

  for(;;) {
    led_toggle(LED_GREEN);

    osDelay(500);
  }
}

static status_t log_tx_string(const char *p)
{
  return log_uart->uart_tx((uint8_t *)p, strlen((const char *)p));
}

/******************** kyChu<kyChu@qq.com> **** END OF FILE ********************/
