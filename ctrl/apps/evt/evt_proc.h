/*
 * evt_proc.h
 *
 *  Created on: Apr 21, 2020
 *      Author: kychu
 */

#ifndef EVT_EVT_PROC_H_
#define EVT_EVT_PROC_H_

#include "SysConfig.h"

typedef enum {
  btn_evt_release = 0,
  btn_evt_pressed = 1,
  btn_evt_hold_on = 2,
} btn_evt_type_t;

typedef void (*button_event_callback)(int, btn_evt_type_t);

struct btn_cb_t {
  button_event_callback btn_cb;
  struct btn_cb_t *next;
};

void btn_evt_register_callback(struct btn_cb_t *pcb);
void btn_evt_unregister_callback(struct btn_cb_t *pcb);

#endif /* EVT_EVT_PROC_H_ */
