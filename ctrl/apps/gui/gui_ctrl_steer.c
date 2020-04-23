/*
 * gui_config_bl.c
 *
 *  Created on: Apr 22, 2020
 *      Author: kychu
 */

#include "gui_demo.h"

#include "SLIDER.h"

#include "log.h"
#include "steerif.h"
#include "evt_proc.h"

static void ctrl_steer_btn_evt_cb(int id, btn_evt_type_t evt);

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/
extern GUI_CONST_STORAGE GUI_BITMAP bmbackground;

struct btn_cb_t ctrl_steer_btn_evt = {
  ctrl_steer_btn_evt_cb,
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
        steerif_set_poswidth(val * 10 + 1000);
//        ky_info("STEER", "set steer to %d%%", val);
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

static int slider_value = 50;
static int auto_dec_inc = 0;
static int should_exit = 0;

#define CTRL_STEER_SLIDER_HEIGHT       20

void gui_ctrl_steer_start(void)
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
  hSlider = SLIDER_CreateEx(0, (ySize - CTRL_STEER_SLIDER_HEIGHT) >> 1, xSize, CTRL_STEER_SLIDER_HEIGHT, WM_HBKWIN, WM_CF_SHOW | WM_CF_HASTRANS, 0, GUI_ID_SLIDER0);
  SLIDER_SetRange(hSlider, 1, 99);
  SLIDER_SetNumTicks(hSlider, 10);
//  slider_value = dispif_get_backlight() / 10;
  SLIDER_SetValue(hSlider, slider_value);
  WM_SetFocus(hSlider);

  btn_evt_register_callback(&ctrl_steer_btn_evt);
  do {
    GUI_Delay(100);
    if(auto_dec_inc == 1) {
      SLIDER_Inc(hSlider);
      slider_value ++;
    } else if(auto_dec_inc == -1) {
      SLIDER_Dec(hSlider);
      slider_value --;
    }
  } while(should_exit == 0);
  btn_evt_unregister_callback(&ctrl_steer_btn_evt);
  slider_value = 50;
  auto_dec_inc = 0;
  should_exit = 0;
  WM_DeleteWindow(hSlider);
  WM_SetCallback(WM_HBKWIN, pcbPrev);
}

static void ctrl_steer_btn_evt_cb(int id, btn_evt_type_t evt)
{
  if(evt == btn_evt_release) {
    if(id == BTN1) {
      if(auto_dec_inc == 0) {
        if(slider_value > 10)
          slider_value -= 10;
        else
          slider_value = 1;
        SLIDER_SetValue(hSlider, slider_value);
      } else auto_dec_inc = 0;
    }
    if(id == BTN3) {
      if(auto_dec_inc == 0) {
        if(slider_value < 90)
          slider_value += 10;
        else
          slider_value = 99;
        SLIDER_SetValue(hSlider, slider_value);
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
