/*
 * led_task.c
 *
 *  Created on: May 21, 2020
 *      Author: kychu
 */

#include "drivers.h"

#include <string.h>

static const char *TAG = "LED";

static osThreadId led_task_id = NULL;
static uint32_t _task_should_exit = 0;

static void led_thread(void const *argument);

static void led_thread(void const *argument)
{
  (void) argument;

  int led = LED_BLUE;
  led_off(LED_BLUE);
  led_off(LED_GREEN);
  while(!_task_should_exit) {
    led_on(led);
    delay(150);
    led_off(led);
    delay(150);
    led_on(led);
    delay(150);
    led_off(led);
    delay(600);
    if(led == LED_BLUE) led = LED_GREEN;
    else led = LED_BLUE;
  }
  led_off(LED_BLUE);
  led_off(LED_GREEN);
  led_task_id = NULL;
  vTaskDelete(NULL);
}

/* LED INDICATOR TASK */
osThreadDef(LEDS, led_thread, osPriorityNormal, 0, 128); // stack size = 128B

int led_main(int argc, char *argv[])
{
  if(argc < 2) return -1;
  if(!strcmp(argv[1], "ctrl")) {
    if(argc < 4) return -1;
    int led = -1;
    if(!strcmp(argv[2], "blue")) {
      led = LED_BLUE;
    } else if(!(strcmp(argv[2], "green"))) {
      led = LED_GREEN;
    }
    if(led != -1) {
      if(!strcmp(argv[3], "on")) {
        led_on(led);
        return 0;
      }
      if(!strcmp(argv[3], "off")) {
        led_off(led);
        return 0;
      }
      if(!strcmp(argv[3], "tog")) {
        led_toggle(led);
        return 0;
      }
    }
    return -1;
  }
  if(!strcmp(argv[1], "task")) {
    if(argc < 3) return -1;
    if(!strcmp(argv[2], "start")) {
      if(led_task_id == NULL) {
        _task_should_exit = 0;
        led_task_id = osThreadCreate(osThread(LEDS), NULL);
        if(led_task_id == NULL) {
          ky_err(TAG, "task create failed.");
          return -2;
        }
      } else {
        ky_warn(TAG, "already started");
      }
      return 0;
    }
    if(!strcmp(argv[2], "stop")) {
      if(led_task_id != NULL) {
        _task_should_exit = 1;
        return 0;
      }
    }
    return -1;
  }
  return -1;
}
