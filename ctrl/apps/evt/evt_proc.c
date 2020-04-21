/*
 * evt_proc.c
 *
 *  Created on: Apr 14, 2020
 *      Author: kychu
 */

#include "apps.h"

#include "evt_proc.h"

const static char *TAG = "EVTP";

#define BUTTON_EVENT_CALLBACK_TEST               (1)

#define BTN_STATE_LAST_STATE   0x01 /* this bit used to hold last button state */
#define BTN_STATE_PRES_STATE   0x02
#define BTN_STATE_HOLD_STATE   0x04
#define BTN_STATE_RELS_STATE   0x08

struct BtnState {
  uint32_t state;
  uint32_t timestamp;
};

static struct btn_cb_t *btn_cb_root = NULL;

static void btn_event_cb(int id, btn_evt_type_t evt);
#if BUTTON_EVENT_CALLBACK_TEST
static void button_event_test_cb(int id, btn_evt_type_t evt);

static struct btn_cb_t test_cb = {
  button_event_test_cb,
  NULL
};
#endif /* BUTTON_EVENT_CALLBACK_TEST */

void evt_proc_task(void const *argument)
{
  uint32_t task_timestamp;
  struct BtnState *btn_state;
  btn_state = kmm_alloc(sizeof(struct BtnState) * NBTNS);
  if(btn_state == NULL) {
    ky_err(TAG, "[btn] no memory");
    vTaskDelete(NULL);
  }
  uint32_t btn_sta = 0;
  buttons_init();
  task_timestamp = xTaskGetTickCountFromISR();
  for(int i = 0; i < NBTNS; i ++) {
    btn_state[i].state = 0;
    btn_state[i].timestamp = task_timestamp;
    btn_state[i].state |= BTN_STATE_RELS_STATE; // indicate that buttons already released.
  }
#if BUTTON_EVENT_CALLBACK_TEST
  btn_evt_register_callback(&test_cb);
#endif /* BUTTON_EVENT_CALLBACK_TEST */
  for(;;) {
    task_timestamp = xTaskGetTickCountFromISR(); // get current task timestamp
    for(int i = 0; i < NBTNS; i ++) {
      btn_sta = button_read(i); // read current state of button
      if(btn_sta == btn_pressed) { // if button pressed
        if((btn_state[i].state & BTN_STATE_LAST_STATE) == 0) { // the first time pressed
          btn_state[i].state |= BTN_STATE_LAST_STATE;
          btn_state[i].state &= ~BTN_STATE_PRES_STATE;
          btn_state[i].state &= ~BTN_STATE_HOLD_STATE;
          btn_state[i].timestamp = task_timestamp; // save the current timestamp
        } else { // press state confirmed
          if((btn_state[i].state & BTN_STATE_PRES_STATE) == 0) { // send pressed event once
            btn_state[i].state |= BTN_STATE_PRES_STATE;
            btn_event_cb(i, btn_evt_pressed);
          }
          if((task_timestamp - btn_state[i].timestamp) > 1000) { // if button has been pressed more than 1s
            if((btn_state[i].state & BTN_STATE_HOLD_STATE) == 0) { // send hold on event once
              btn_state[i].state |= BTN_STATE_HOLD_STATE;
              btn_event_cb(i, btn_evt_hold_on);
            }
          }
        }
      } else if(btn_sta == btn_released) { // if button released
        if((btn_state[i].state & BTN_STATE_LAST_STATE) != 0) { // the first time released
          btn_state[i].state &= ~BTN_STATE_LAST_STATE;
          btn_state[i].state &= ~BTN_STATE_RELS_STATE;
        } else { // release state confirmed
          if((btn_state[i].state & BTN_STATE_RELS_STATE) == 0) { // notify that button was released
            btn_state[i].state |= BTN_STATE_RELS_STATE;
            btn_event_cb(i, btn_evt_release);
          }
        }
      }
    }
    // detect period is 50ms
    delay(50);
  }
}

void btn_evt_register_callback(struct btn_cb_t *pcb)
{
  struct btn_cb_t *p = btn_cb_root;
  if(p == NULL) {
    btn_cb_root = pcb;
  } else {
    for(; p->next != NULL; p = p->next) {}
    p->next = pcb;
  }
  pcb->next = NULL; // reset list tail to avoid infinite recursion
}

void btn_evt_unregister_callback(struct btn_cb_t *pcb)
{
  struct btn_cb_t *p = btn_cb_root;
  if(p == pcb) {
    btn_cb_root = pcb->next; // remove root
  } else if(p != NULL) {
    for(; (p->next != pcb) && (p->next != NULL); p = p->next) {}
    if(p->next != NULL) { // next is the object who need to remove
      p->next = pcb->next;
      pcb->next = NULL;
    } // dose not exist in list.
  } // root is null
}

static void btn_event_cb(int id, btn_evt_type_t evt)
{
  struct btn_cb_t *p = btn_cb_root;
  for(; p != NULL; p = p->next) {
    if(p->btn_cb != NULL) {
      p->btn_cb(id, evt);
    }
  }
}

#if BUTTON_EVENT_CALLBACK_TEST

static void button_event_test_cb(int id, btn_evt_type_t evt)
{
  switch(evt) {
  case btn_evt_release:
    ky_info(TAG, "button %d released", id);
  break;
  case btn_evt_pressed:
    ky_info(TAG, "button %d pressed", id);
  break;
  case btn_evt_hold_on:
    ky_info(TAG, "button %d hold on", id);
  break;
  }
}

#endif /* BUTTON_EVENT_CALLBACK_TEST */
