/*
 * gui_graph.c
 *
 *  Created on: Apr 21, 2020
 *      Author: kychu
 */

#include "gui_demo.h"

#include "FRAMEWIN.h"
#include "GRAPH.h"

#include "log.h"
#include "evt_proc.h"
#include "cpu_utils.h"

#include "ctrl_task.h"
#include "att_est_q.h"
#include "rf_evt_proc.h"

//static const char *TAG = "WAVE";

#define GRAPH_SHOW_SCALE_HORIZON       (0)

#define BK_COLOR_0        0xFF5555
#define BK_COLOR_1        0x880000

#define DIST_TO_WIN      5

#define BORDER_TOP       0
#if GRAPH_SHOW_SCALE_HORIZON
#define BORDER_BOTTOM    9
#else
#define BORDER_BOTTOM    0
#endif /* GRAPH_SHOW_SCALE_HORIZON */
#define BORDER_LEFT      30
#define BORDER_RIGHT     0

#if GRAPH_SHOW_SCALE_HORIZON
#define SCALE_H_POS      4
#endif /* GRAPH_SHOW_SCALE_HORIZON */
#define SCALE_V_POS      (BORDER_LEFT - 9)

#define COLOR_BK         GUI_DARKGRAY
#define COLOR_BORDER     BK_COLOR_1
#define COLOR_FRAME      GUI_BLACK
#define COLOR_GRID       GUI_GRAY

#if GRAPH_SHOW_SCALE_HORIZON
#define SCALE_H_HEIGHT   4 // height if horizontal scale
#endif /* GRAPH_SHOW_SCALE_HORIZON */

#define TICK_DIST_H      25
#define TICK_DIST_V      20

#define TIME_RUN         20000
#define TIME_STEP        15

#define GRID_DIST_X      25
#define GRID_DIST_Y      10

#define WAVE_NUMBER      5
#define MAX_NUM_DATA_OBJ 5 // max waves we support

static void graph_btn_evt_cb(int id, btn_evt_type_t evt);

struct btn_cb_t graph_btn_evt = {
  graph_btn_evt_cb,
  NULL
};

static Euler_T euler;

/*********************************************************************
*
*       Typedef / Data
*
**********************************************************************
*/
typedef struct {
  const char *Title;
  int         ScaleVOff;
  void     (* pfAddData)(GRAPH_DATA_Handle hData, int WaveID);
  int         NumWaves;
  int         DataPeriod;
  float       DataFactor;
} GraphWaveDef;

static GUI_COLOR _aColorData[MAX_NUM_DATA_OBJ] = {
  0x50C0FF,
  0xFFC050,
  0x50FFC0,
  0x800000,
  0x000080
};

#if GRAPH_SHOW_SCALE_HORIZON
GRAPH_SCALE_Handle _hScaleH;
#endif /* GRAPH_SHOW_SCALE_HORIZON */
GRAPH_SCALE_Handle _hScaleV;
static int         _DataOffset;
static float       _DataFactor;
static char*       _DataTitle;

/*********************************************************************
*
*       Static functions
*
**********************************************************************
*/
static void _AddData_Euler(GRAPH_DATA_Handle hData, int DataID) {
  if(DataID == 0) {
    att_est_get_euler(&euler);
    GRAPH_DATA_YT_AddValue(hData, euler.pitch + _DataOffset);//Index osGetCPUUsage()
  } else {
    GRAPH_DATA_YT_AddValue(hData, euler.roll + _DataOffset);
  }
}

static void _AddData_Usage(GRAPH_DATA_Handle hData, int DataID) {
  GUI_USE_PARA(DataID);
  GRAPH_DATA_YT_AddValue(hData, osGetCPUUsage() + _DataOffset);
}

static void _AddData_Voltage(GRAPH_DATA_Handle hData, int DataID) {
  GUI_USE_PARA(DataID);
  GRAPH_DATA_YT_AddValue(hData, (I16)(battery_volt_read() / _DataFactor + _DataOffset));
}

static void _AddData_Remote(GRAPH_DATA_Handle hData, int DataID) {
  switch(DataID) {
  case 0:
    GRAPH_DATA_YT_AddValue(hData, (I16)(rmt_get_pit_chan() / _DataFactor + _DataOffset)); break;
  case 1:
    GRAPH_DATA_YT_AddValue(hData, (I16)(rmt_get_rol_chan() / _DataFactor + _DataOffset)); break;
  case 2:
    GRAPH_DATA_YT_AddValue(hData, (I16)(rmt_get_yaw_chan() / _DataFactor + _DataOffset)); break;
  case 3:
    GRAPH_DATA_YT_AddValue(hData, (I16)(rmt_get_thr_chan() / _DataFactor + _DataOffset)); break;
  default: break;
  }
}

static void _AddData_MotCtrl(GRAPH_DATA_Handle hData, int DataID) {
  if(DataID == 0) GRAPH_DATA_YT_AddValue(hData, (I16)(ctrl_get_motor_exp() / _DataFactor + _DataOffset));
  if(DataID == 1) GRAPH_DATA_YT_AddValue(hData, (I16)(ctrl_get_motor_vel() / _DataFactor + _DataOffset));
}

/*********************************************************************
*
*       DATA _aWave - Keep below _AddData-functions
*/
static const GraphWaveDef WaveList[WAVE_NUMBER] = {
  {
    "Euler Angle",
    35,
    _AddData_Euler,
    2,
	20,            /* 20ms */
	1,
  },
  {
    "CPU Usage",
	0,
	_AddData_Usage,
	1,
	50,            /* 50ms */
	1,
  },
  {
    "Battery Volt",
	-80,
	_AddData_Voltage,
	1,
	50,            /* 50ms */
	0.1f,
  },
  {
    "RF Channel",
	4,
	_AddData_Remote,
	4,
	50,
	(1200.0f / (80 - BORDER_BOTTOM)),
  },
  {
    "Motor Control",
    0, //	(80 - BORDER_BOTTOM) / 2 - 4,
	_AddData_MotCtrl,
	2,
	50,
	(2500.0f / (80 - BORDER_BOTTOM)),
  },
};

/*********************************************************************
*
*       _UserDraw
*
* Function description
*   This routine is called by the GRAPH object before anything is drawn
*   and after the last drawing operation.
*/
static void _UserDraw(WM_HWIN hWin, int Stage) {
  if (Stage == GRAPH_DRAW_LAST) {
//    char acText[] = "Temperature";
    GUI_RECT Rect;
    GUI_RECT RectInvalid;
    int FontSizeY;

    GUI_SetFont(&GUI_Font13_ASCII);
    FontSizeY = GUI_GetFontSizeY();
    WM_GetInsideRect(&Rect);
    WM_GetInvalidRect(hWin, &RectInvalid);
    Rect.x1 = Rect.x0 + FontSizeY;
    GUI_SetColor(GUI_YELLOW);
    GUI_DispStringInRectEx(_DataTitle, &Rect, GUI_TA_HCENTER, strlen(_DataTitle), GUI_ROTATE_CCW);
  }
}

/*********************************************************************
*
*       _cbBk
*/
static void _cbBk(WM_MESSAGE * pMsg) {
  switch (pMsg->MsgId) {
  case WM_PAINT:
    GUI_SetBkColor(BK_COLOR_1);
    GUI_Clear();
  break;
  default:
    WM_DefaultProc(pMsg);
  }
}

static int select_wave = 0;
static int should_exit = 0;

void gui_graph_start(void)
{
  const WIDGET_EFFECT * pEffectOld;
  GRAPH_Handle          hGraph;
  GRAPH_DATA_Handle     hData[MAX_NUM_DATA_OBJ];
  int                   xSize, ySize, i, current_wave_id;
  GUI_TIMER_TIME        TimeStart, TimeDiff;

  xSize      = LCD_GetXSize();
  ySize      = LCD_GetYSize();
  pEffectOld = WIDGET_SetDefaultEffect(&WIDGET_Effect_Simple);
  //
  // Set Callback function for background window
  //
  WM_SetCallback(WM_HBKWIN, _cbBk);
  hGraph = GRAPH_CreateEx(0, 0, xSize, ySize, WM_HBKWIN, WM_CF_SHOW | WM_CF_CONST_OUTLINE, 0, GUI_ID_GRAPH0);
  GRAPH_SetBorder(hGraph, BORDER_LEFT, BORDER_TOP, BORDER_RIGHT, BORDER_BOTTOM);
  WM_SetHasTrans (hGraph);
  GRAPH_SetColor (hGraph, COLOR_BK,     GRAPH_CI_BK);
  GRAPH_SetColor (hGraph, COLOR_BORDER, GRAPH_CI_BORDER);
  GRAPH_SetColor (hGraph, COLOR_FRAME,  GRAPH_CI_FRAME);
  GRAPH_SetColor (hGraph, COLOR_GRID,   GRAPH_CI_GRID);
  //
  // Adjust grid
  //
  GRAPH_SetGridVis  (hGraph, 1);
  GRAPH_SetGridDistX(hGraph, GRID_DIST_X);
  GRAPH_SetGridDistY(hGraph, GRID_DIST_Y);
  WM_BringToBottom  (hGraph);
  //
  // Create and configure GRAPH_DATA_YT object
  //
  for (i = 0; i < MAX_NUM_DATA_OBJ; i++) {
    hData[i] = GRAPH_DATA_YT_Create(_aColorData[i], xSize - (BORDER_LEFT + BORDER_RIGHT), 0, 0);
  }
  //
  // Create and configure GRAPH_SCALE objects
  //
  _hScaleV = GRAPH_SCALE_Create(SCALE_V_POS, GUI_TA_HCENTER, GRAPH_SCALE_CF_VERTICAL,   TICK_DIST_V);
#if GRAPH_SHOW_SCALE_HORIZON
  _hScaleH = GRAPH_SCALE_Create(SCALE_H_POS, GUI_TA_VCENTER, GRAPH_SCALE_CF_HORIZONTAL, TICK_DIST_H);
  GRAPH_SCALE_SetPos(_hScaleH, ySize - SCALE_H_HEIGHT);
  GRAPH_SCALE_SetOff(_hScaleH, -5);
#endif /* GRAPH_SHOW_SCALE_HORIZON */

  GRAPH_SetUserDraw(hGraph, _UserDraw);

  btn_evt_register_callback(&graph_btn_evt);
  //
  // Show graphs
  //
  do {
    current_wave_id = select_wave;
    _DataTitle = (char *)WaveList[current_wave_id].Title;
//    ky_info(TAG, "Show Wave: %s", WaveList[current_wave_id].Title);

    _DataOffset = WaveList[current_wave_id].ScaleVOff;
    _DataFactor = WaveList[current_wave_id].DataFactor;
    GRAPH_SetGridOffY (hGraph, _DataOffset % GRID_DIST_Y);
    GRAPH_SCALE_SetOff(_hScaleV, _DataOffset);
    GRAPH_SCALE_SetFactor(_hScaleV, _DataFactor);

    // attach horizontal & vertical scale
#if GRAPH_SHOW_SCALE_HORIZON
    GRAPH_AttachScale(hGraph, _hScaleH);
#endif /* GRAPH_SHOW_SCALE_HORIZON */
    GRAPH_AttachScale(hGraph, _hScaleV);

    for(i = 0; (i < WaveList[current_wave_id].NumWaves) && (i < MAX_NUM_DATA_OBJ); i ++) {
      GRAPH_AttachData(hGraph, hData[i]);
    }

    GUI_Exec();
#if GRAPH_SHOW_SCALE_HORIZON
    GRAPH_DetachScale(hGraph, _hScaleH);
#endif /* GRAPH_SHOW_SCALE_HORIZON */
    GRAPH_DetachScale(hGraph, _hScaleV);
    WM_ValidateWindow(hGraph);

    do {
      TimeStart = GUI_GetTime();
      for(i = 0; (i < WaveList[current_wave_id].NumWaves) && (i < MAX_NUM_DATA_OBJ); i ++) {
        WaveList[current_wave_id].pfAddData(hData[i], i);
      }
      TimeDiff = GUI_GetTime() - TimeStart;
      if(TimeDiff < WaveList[current_wave_id].DataPeriod) {
        GUI_Delay(WaveList[current_wave_id].DataPeriod - TimeDiff);
      }
    } while((should_exit == 0) && (current_wave_id == select_wave));

    for(i = 0; (i < WaveList[current_wave_id].NumWaves) && (i < MAX_NUM_DATA_OBJ); i ++) {
      GRAPH_DetachData(hGraph, hData[i]);
    }
  } while(should_exit == 0);

  btn_evt_unregister_callback(&graph_btn_evt);
  should_exit = 0;
  //
  // Clean up
  //
#if GRAPH_SHOW_SCALE_HORIZON
  GRAPH_DetachScale(hGraph, _hScaleH);
#endif /* GRAPH_SHOW_SCALE_HORIZON */
  GRAPH_DetachScale(hGraph, _hScaleV);
#if GRAPH_SHOW_SCALE_HORIZON
  GRAPH_SCALE_Delete(_hScaleH);
#endif /* GRAPH_SHOW_SCALE_HORIZON */
  GRAPH_SCALE_Delete(_hScaleV);
  for (i = 0; i < MAX_NUM_DATA_OBJ; i++) {
    GRAPH_DATA_YT_Delete(hData[i]);
  }
  WM_DeleteWindow(hGraph);
  WIDGET_SetDefaultEffect(pEffectOld);
}

static void graph_btn_evt_cb(int id, btn_evt_type_t evt)
{
  if(evt == btn_evt_release) {
    if(id == BTN_CENTER) {
      should_exit = 1;
    }
    if(id == BTN_LEFT) {
      if(select_wave > 0) select_wave --;
      else select_wave = WAVE_NUMBER - 1;
    }
    if(id == BTN_RIGHT) {
      if(select_wave < (WAVE_NUMBER - 1)) select_wave ++;
      else select_wave = 0;
    }
  }
}
