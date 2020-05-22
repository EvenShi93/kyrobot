/*
 * gui_iconview.c
 *
 *  Created on: Apr 22, 2020
 *      Author: kychu
 */

#include "gui_demo.h"

#include "TEXT.h"
#include "ICONVIEW.h"

#include "log.h"

#include "evt_proc.h"

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/

static const char *TAG = "MENU";

#define MSG_BATTERY_ICON (WM_USER + 0)

static void iconview_btn_evt_cb(int id, btn_evt_type_t evt);

/*********************************************************************
*
*       Types
*
**********************************************************************
*/
typedef struct {
  const GUI_BITMAP * pBitmap;
  const char       * pText;
  const char       * pExplanation;
  const int          optCode;
} BITMAP_ITEM;

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/
extern GUI_CONST_STORAGE GUI_BITMAP bmbackground;

struct btn_cb_t icon_btn_evt = {
  iconview_btn_evt_cb,
  NULL
};

#include "icon_source.c"

/*********************************************************************
*
*       _aBitmapItem
*/
static const BITMAP_ITEM _aBitmapItem[] = {
  {&_bmWaveTool,"Waves"   , "Show Waves on displayer", menu_code_waves},
  {&_bmSystem,  "System"  , "Change system settings", menu_code_system},
//  {&_bmDate,    "Date"    , "Use the diary"},
//  {&_bmEmail,   "Email"   , "Read an email"},
//  {&_bmRead,    "Read"    , "Read a document"},
//  {&_bmWrite,   "Write"   , "Write an email"},
//  {&_bmPassword,"Password", "Determine the system password", 2},
  {&_bmRemote,  "Network" , "Select network", menu_code_nothing},
//  {&_bmClock,   "Clock"   , "Adjust current time and date", menu_code_nothing},
  {&_bmPower,   "Power"   , "Power Off", menu_code_poweroff}
};

static int battery_level = 4;

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
  WM_HWIN hItem;
  GUI_RECT Rect;
  switch (pMsg->MsgId) {
  case WM_NOTIFY_PARENT:
    Id    = WM_GetId(pMsg->hWinSrc);      /* Id of widget */
    NCode = pMsg->Data.v;                 /* Notification code */
    switch (Id) {
    case GUI_ID_ICONVIEW0:
      switch (NCode) {
      case WM_NOTIFICATION_SEL_CHANGED:
        //
        // Change widget text changing the selection
        //
        Sel   = ICONVIEW_GetSel(pMsg->hWinSrc);
        hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_TEXT0);
        TEXT_SetText(hItem, _aBitmapItem[Sel].pExplanation);
        ky_info(TAG, "%s", _aBitmapItem[Sel].pExplanation);
        break;
      }
      break;
    }
    break;
  case MSG_BATTERY_ICON:

	  Rect.x0 = LCD_GetXSize() - ((40 + bmbl_4.XSize) / 2);
	  Rect.y0 = 5;
	  Rect.x1 = Rect.x0 + bmbl_4.XSize;
	  Rect.y1 = Rect.y0 + bmbl_4.YSize;
	  WM_InvalidateRect(pMsg->hWin, &Rect);
  break;
  case WM_PAINT:
    //
    // Draw background
    //
    GUI_DrawBitmap(&bmbackground, 0, 0);
    switch(battery_level) {
    case 4:
    GUI_DrawBitmap(&bmbl_4, LCD_GetXSize() - ((40 + bmbl_4.XSize) / 2), 5);
    break;
    case 3:
    	GUI_DrawBitmap(&bmbl_3, LCD_GetXSize() - ((40 + bmbl_3.XSize) / 2), 5);
    break;
    case 2:
    	GUI_DrawBitmap(&bmbl_2, LCD_GetXSize() - ((40 + bmbl_2.XSize) / 2), 5);
    break;
    case 1:
    default:
    	GUI_DrawBitmap(&bmbl_1, LCD_GetXSize() - ((40 + bmbl_1.XSize) / 2), 5);
    break;
    }
    break;
  }
}

static WM_HWIN hIconView;

static int cur_sel = 0;
static int should_exit = 0;

/*********************************************************************
*
*       Public routines
*
**********************************************************************
*/
/*********************************************************************
*
*       GUIDEMO_IconView
*/
main_menu_code_t gui_iconview_start(void) {
  WM_CALLBACK * pcbPrev;
  WM_HWIN       hText;
  int           i, xSize, ySize;
  float         battery_voltage;

  WM_EnableMemdev(WM_HBKWIN);
  pcbPrev = WM_SetCallback(WM_HBKWIN, _cbWin);
  //
  // Create iconview widget
  //
  xSize = LCD_GetXSize();
  ySize = LCD_GetYSize();
  hIconView = ICONVIEW_CreateEx(0, 0, xSize - 40, ySize - 14,
                           WM_HBKWIN, WM_CF_SHOW | WM_CF_HASTRANS,
                           0, GUI_ID_ICONVIEW0, 55, 60);
  for (i = 0; i < GUI_COUNTOF(_aBitmapItem); i++) {
    //
    // Add icons to the widget
    //
    ICONVIEW_AddBitmapItem(hIconView, _aBitmapItem[i].pBitmap, _aBitmapItem[i].pText);
  }
  ICONVIEW_SetBkColor(hIconView, ICONVIEW_CI_SEL, GUI_BLUE | 0xC0000000);
  ICONVIEW_SetFont(hIconView, &GUI_Font13B_ASCII);
  WM_SetFocus(hIconView);

  //
  // Create explanation
  //
  hText = TEXT_CreateEx(0, ySize - 14, 160, 14, WM_HBKWIN, WM_CF_SHOW, 0, GUI_ID_TEXT0, "");
  TEXT_SetFont(hText, &GUI_Font13_ASCII);
  TEXT_SetTextColor(hText, GUI_WHITE);
  TEXT_SetWrapMode(hText, GUI_WRAPMODE_WORD);
  TEXT_SetTextAlign(hText, GUI_TA_HCENTER | GUI_TA_VCENTER);

  ICONVIEW_SetSel(hIconView, cur_sel);
  TEXT_SetText(hText, _aBitmapItem[cur_sel].pExplanation);
  btn_evt_register_callback(&icon_btn_evt);
  do {
    GUI_Delay(50);
    battery_voltage = battery_volt_read();
    if(battery_voltage > 12.0f) {
      if(battery_level != 4) {
        battery_level = 4;
        WM_SendMessageNoPara(WM_HBKWIN, MSG_BATTERY_ICON);
      }
    } else if(battery_voltage > 11.0f) {
    	if(battery_level != 3) {
    	        battery_level = 3;
    	        WM_SendMessageNoPara(WM_HBKWIN, MSG_BATTERY_ICON);
    	      }
    } else if(battery_voltage > 10.0f) {
    	if(battery_level != 2) {
    	        battery_level = 2;
    	        WM_SendMessageNoPara(WM_HBKWIN, MSG_BATTERY_ICON);
    	      }
    } else {
    	if(battery_level != 1) {
    	        battery_level = 1;
    	        WM_SendMessageNoPara(WM_HBKWIN, MSG_BATTERY_ICON);
    	      }
    }
  } while(should_exit == 0);
  btn_evt_unregister_callback(&icon_btn_evt);
  should_exit = 0;
  WM_DeleteWindow(hIconView);
  WM_DeleteWindow(hText);
  WM_SetCallback(WM_HBKWIN, pcbPrev);
  return _aBitmapItem[cur_sel].optCode;
}

static void iconview_btn_evt_cb(int id, btn_evt_type_t evt)
{
  if(evt == btn_evt_release) {
    if(id == BTN1) {
      if(cur_sel > 0) {cur_sel --;
        ICONVIEW_SetSel(hIconView, cur_sel);
      }
    }
    if(id == BTN3) {
      if(cur_sel < (GUI_COUNTOF(_aBitmapItem) - 1)) {
        cur_sel ++;
        ICONVIEW_SetSel(hIconView, cur_sel);
      }
    }
    if(id == BTN2) {
      should_exit = 1;
    }
  }
}
