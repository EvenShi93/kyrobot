/*
 * gui_config_bl.c
 *
 *  Created on: Apr 22, 2020
 *      Author: kychu
 */

#include "gui_demo.h"

#include "SLIDER.h"

#include "log.h"
#include "motorif.h"
#include "evt_proc.h"

static void auto_slider_step(int flag);
static void ctrl_motor_btn_evt_cb(int id, btn_evt_type_t evt);

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/
extern GUI_CONST_STORAGE GUI_BITMAP bmbackground;

struct btn_cb_t ctrl_motor_btn_evt = {
  ctrl_motor_btn_evt_cb,
  NULL
};

/*********************************************************************
*
*       Private routines
*
**********************************************************************
*/
/*********************************************************************
*
*       _cbWin
*/
static void _cbWin(WM_MESSAGE * pMsg) {
  int NCode, Id, val;
  switch (pMsg->MsgId) {
  case WM_NOTIFY_PARENT:
    Id    = WM_GetId(pMsg->hWinSrc);      /* Id of widget */
    NCode = pMsg->Data.v;                 /* Notification code */
    switch (Id) {
    case GUI_ID_SLIDER0:
      switch (NCode) {
      case WM_NOTIFICATION_VALUE_CHANGED:
        //
        // Change widget text changing the selection
        //
        val   = SLIDER_GetValue(pMsg->hWinSrc);
//        dispif_set_backlight(val * 10);
        if(val > 0) {
          motorif_set_dutycycle_a(val * 10);
          motorif_set_dutycycle_b(0);
        } else {
          motorif_set_dutycycle_a(0);
          motorif_set_dutycycle_b(-val * 10);
        }
        ky_info("MOTOR", "set motor to %d%%", val);
      break;
      }
    break;
    }
  break;
  case WM_PAINT:
    //
    // Draw background
    //
    GUI_DrawBitmap(&bmbackground, 0, 0);
  break;
  }
}

static WM_HWIN hSlider;

static int slider_value = 0;
static int auto_dec_inc = 0;
static int should_exit = 0;

#define CTRL_MOTOR_SLIDER_HEIGHT       20

void gui_ctrl_motor_start(void)
{
  WM_CALLBACK * pcbPrev;
  int           xSize, ySize;

  WM_EnableMemdev(WM_HBKWIN);
  pcbPrev = WM_SetCallback(WM_HBKWIN, _cbWin);

  //
  // Create slider widget
  //
  xSize = LCD_GetXSize();
  ySize = LCD_GetYSize();
  hSlider = SLIDER_CreateEx(0, (ySize - CTRL_MOTOR_SLIDER_HEIGHT) >> 1, xSize, CTRL_MOTOR_SLIDER_HEIGHT, WM_HBKWIN, WM_CF_SHOW | WM_CF_HASTRANS, 0, GUI_ID_SLIDER0);
  SLIDER_SetRange(hSlider, -100, 100);
  SLIDER_SetNumTicks(hSlider, 10);
  slider_value = 0;
  SLIDER_SetValue(hSlider, 0);
  WM_SetFocus(hSlider);

  btn_evt_register_callback(&ctrl_motor_btn_evt);
  do {
    GUI_Delay(100);
    auto_slider_step(auto_dec_inc);
  } while(should_exit == 0);
  btn_evt_unregister_callback(&ctrl_motor_btn_evt);
  motorif_set_dutycycle_a(0); // stop motor
  motorif_set_dutycycle_b(0);
  auto_dec_inc = 0;
  should_exit = 0;
  WM_DeleteWindow(hSlider);
  WM_SetCallback(WM_HBKWIN, pcbPrev);
}

#define SLIDER_ADJ_STEP      5

static void auto_slider_step(int flag)
{
  if(flag != 0) {
    if(flag > 0) {
      if((slider_value + SLIDER_ADJ_STEP) <= 100)
        slider_value += SLIDER_ADJ_STEP;
      else
        slider_value = 100;
    } else {
      if((slider_value - SLIDER_ADJ_STEP) >= -100)
        slider_value -= SLIDER_ADJ_STEP;
      else
        slider_value = -100;
    }
    SLIDER_SetValue(hSlider, slider_value);
  }
}

static void ctrl_motor_btn_evt_cb(int id, btn_evt_type_t evt)
{
  if(evt == btn_evt_release) {
    if(id == BTN1) {
      if(auto_dec_inc == 0) {
        auto_slider_step(-1);
      } else auto_dec_inc = 0;
    }
    if(id == BTN3) {
      if(auto_dec_inc == 0) {
        auto_slider_step(1);
      } else auto_dec_inc = 0;
    }
    if(id == BTN2) {
      should_exit = 1;
    }
  }
  if(evt == btn_evt_hold_on) {
    if(id == BTN1) {
      auto_dec_inc = -1;
    }
    if(id == BTN3) {
      auto_dec_inc = 1;
    }
  }
}
