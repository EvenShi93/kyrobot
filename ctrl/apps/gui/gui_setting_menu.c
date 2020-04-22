/*
 * gui_setting_menu.c
 *
 *  Created on: Apr 22, 2020
 *      Author: kychu
 */

#include "gui_demo.h"

#include "LISTBOX.h"

#include "log.h"

#include "evt_proc.h"

static void settingmenu_btn_evt_cb(int id, btn_evt_type_t evt);

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/
extern GUI_CONST_STORAGE GUI_BITMAP bmbackground;

struct btn_cb_t settingmenu_btn_evt = {
  settingmenu_btn_evt_cb,
  NULL
};

static GUI_ConstString pMenuTexts[] = {
  "LISTBOXTEXT0",
  "LISTBOXTEXT1",
  "LISTBOXTEXT2",
  "LISTBOXTEXT3",
  "LISTBOXTEXT4",
  "LISTBOXTEXT5",
  0
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
  int NCode, Id, Sel;
  switch (pMsg->MsgId) {
  case WM_NOTIFY_PARENT:
    Id    = WM_GetId(pMsg->hWinSrc);      /* Id of widget */
    NCode = pMsg->Data.v;                 /* Notification code */
    switch (Id) {
    case GUI_ID_LISTBOX0:
      switch (NCode) {
      case WM_NOTIFICATION_SEL_CHANGED:
        //
        // Change widget text changing the selection
        //
        Sel   = LISTBOX_GetSel(pMsg->hWinSrc);
        ky_info("MENU", "%s", pMenuTexts[Sel]);
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

static WM_HWIN hListBox;

static int should_exit = 0;

void gui_setting_menu_start(void)
{
  WM_CALLBACK * pcbPrev;
  int           xSize, ySize;

  WM_EnableMemdev(WM_HBKWIN);
  pcbPrev = WM_SetCallback(WM_HBKWIN, _cbWin);
  //
  // Create listbox widget
  //
  xSize = LCD_GetXSize();
  ySize = LCD_GetYSize();
  hListBox = LISTBOX_CreateEx(0, 0, xSize, ySize, WM_HBKWIN, WM_CF_SHOW, 0, GUI_ID_LISTBOX0, pMenuTexts);
  LISTBOX_SetDefaultBkColor(hListBox, GUI_GRAY);
  LISTBOX_SetFont(hListBox, &GUI_Font16B_ASCII);

  WM_SetFocus(hListBox);

  btn_evt_register_callback(&settingmenu_btn_evt);
  do {
    GUI_Delay(50);
  } while(should_exit == 0);
  btn_evt_unregister_callback(&settingmenu_btn_evt);
  should_exit = 0;
  WM_DeleteWindow(hListBox);
  WM_SetCallback(WM_HBKWIN, pcbPrev);
}

static void settingmenu_btn_evt_cb(int id, btn_evt_type_t evt)
{
  if(evt == btn_evt_release) {
    if(id == BTN1) {
      LISTBOX_DecSel(hListBox);
    }
    if(id == BTN3) {
      LISTBOX_IncSel(hListBox);
    }
    if(id == BTN2) {
      should_exit = 1;
    }
  }
}
