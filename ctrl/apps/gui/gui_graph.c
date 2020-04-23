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

#include "att_est_q.h"

#define BK_COLOR_0        0xFF5555
#define BK_COLOR_1        0x880000

//#define DIST_TO_BORDER   0
#define DIST_TO_WIN      5

#define BORDER_TOP       0
#define BORDER_BOTTOM    9
#define BORDER_LEFT      19
#define BORDER_RIGHT     0

#define COLOR_BK         GUI_DARKGRAY
#define COLOR_BORDER     BK_COLOR_1
#define COLOR_FRAME      GUI_BLACK
#define COLOR_GRID       GUI_GRAY

#define SCALE_H_HEIGHT   4 // height if horizontal scale

#define TICK_DIST_H      25
#define TICK_DIST_V      20

#define TIME_RUN         20000
#define TIME_STEP        15

#define MAX_NUM_DATA_OBJ 5 // max waves we support

//#define GRAPH_DIV        9   // (2^9 = 512) If this value is changed _aWaves[] need to be changed too!
#define GRID_DIST_X      25
#define GRID_DIST_Y      10
//#define GRID_OFF_Y       25

static void graph_btn_evt_cb(int id, btn_evt_type_t evt);

extern GUI_CONST_STORAGE GUI_BITMAP bmbackground;

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
//typedef struct {
//  int     ScaleVOff;
//  int     DataVOff;
//  int     GridVOff;
//  void (* pfAddData)(GRAPH_DATA_Handle hData, int DataID);
//  int     NumWaves;
//} GRAPH_WAVE;

typedef struct {
  const char *Title;
  int         ScaleVOff;
  void     (* pfAddData)(GRAPH_DATA_Handle hData, int WaveID);
  int         NumWaves;
} GraphWaveDef;

//static int _HeartBeat[] = {
//    2,   4,   6,   8,  10,   6,   2,   0,   0,   0,
//   -8,  16,  40,  64,  88,  58,  28,  -2, -32, -30,
//  -20, -10,   0,   2,   2,   4,   4,   6,   6,   8,
//    8,  10,  12,  14,  16,  18,  20,  16,  12,   8,
//    4,   2,   2,   0,   0,   0,   0,   0,   0,   0,
//    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
//    0,   0,   0,   0,   0,   0,   0,   0,   0,   0
//};

static GUI_COLOR _aColorData[MAX_NUM_DATA_OBJ] = {
  0x50C0FF,
  0xFFC050,
  0x50FFC0,
  0x800000,
  0x000080
};

GRAPH_SCALE_Handle _hScaleH, _hScaleV;
static int         _DataAdjust;

/*********************************************************************
*
*       Static functions
*
**********************************************************************
*/
/*********************************************************************
*
*       _AddData_Heartbeat
*/
static void _AddData_Heartbeat(GRAPH_DATA_Handle hData, int DataID) {
//  static int Index = 0;

//  GRAPH_DATA_YT_AddValue(hData, (_HeartBeat[Index]) + _DataAdjust);
  if(DataID == 0) {
    att_est_get_euler(&euler);
    GRAPH_DATA_YT_AddValue(hData, euler.pitch + _DataAdjust);//Index osGetCPUUsage()
  } else {
    GRAPH_DATA_YT_AddValue(hData, euler.roll + _DataAdjust);
  }
//  if (++Index == GUI_COUNTOF(_HeartBeat)) {
//    Index = 0;
//  }
//  if(++ Index == 30) Index = 0;
}

/*********************************************************************
*
*       DATA _aWave - Keep below _AddData-functions
*/
//GRAPH_WAVE _aWave[] = {
//  {
//    157,                // Vertical scale offset in relation to GRAPH_DIV
//    152,                // Vertical data  offset in relation to GRAPH_DIV
//    21,                 // Vertical grid  offset in relation to GRAPH_DIV
//    _AddData_Heartbeat, // Pointer to specific AddData function
//    1                   // Number of waves
//  },
//  {0}
//};

GraphWaveDef WaveList[] = {
  {
    "Euler",
    30,
    _AddData_Heartbeat,
    2
  },
  {0}
};

/*********************************************************************
*
*       _ShowGraph
*/
//static void _ShowGraph(GRAPH_Handle hGraph, GRAPH_DATA_Handle hData[], int DataCount, void (* pfAddData)(GRAPH_DATA_Handle hData, int DataID)) {
////  int Count, Data_xSize, xSize;
//  int TimeStart, TimeDiff, TimeStep;
//  int i, Flag;
//
////  xSize      = LCD_GetXSize();
////  Data_xSize = xSize - (BORDER_LEFT + BORDER_RIGHT);
////  Count      = 0;
//  //
//  // Attach data objects
//  //
//  for (i = 0; i < DataCount; i++) {
//    GRAPH_AttachData(hGraph, hData[i]);
//  }
//  //
//  // Add values before GRAPH is displayed
//  //
////  while (Count < Data_xSize) {
////    for (i = 0; i < DataCount; i++) {
////      pfAddData(hData[i], i);
////    }
////    Count++;
////  }
//  //
//  // Add values depending on time
//  //
//
//  TimeStart = GUI_GetTime();
//  Flag = 1;
//  do {
//    TimeDiff = GUI_GetTime() - TimeStart;
//    for (i = 0; i < DataCount; i++) {
//      pfAddData(hData[i], i);
//    }
//    if (Flag) {
//      Flag = 0;
//      GUI_Exec();
//      GRAPH_DetachScale(hGraph, _hScaleH);
//      GRAPH_DetachScale(hGraph, _hScaleV);
//      WM_ValidateWindow(hGraph);
//    }
//
//    TimeStep  = GUI_GetTime() - TimeStart;
//    if ((TimeStep - TimeDiff) < TIME_STEP) {
//      GUI_Delay(TIME_STEP - (TimeStep - TimeDiff));
//    }
//  } while ((TimeDiff < TIME_RUN));
//  for (i = 0; i < DataCount; i++) {
//    GRAPH_DetachData(hGraph, hData[i]);
//  }
//}

/*********************************************************************
*
*       _cbBk
*/
static void _cbBk(WM_MESSAGE * pMsg) {
  switch (pMsg->MsgId) {
  case WM_PAINT:
//    GUI_SetBkColor(BK_COLOR_1);
//    GUI_Clear();
  //
  // Draw background
  //
  GUI_DrawBitmap(&bmbackground, 0, 0);
  break;
  default:
    WM_DefaultProc(pMsg);
  }
}

static int should_exit = 0;

void gui_graph_start(void)
{
  const WIDGET_EFFECT * pEffectOld;
  GRAPH_Handle          hGraph;
  GRAPH_DATA_Handle     hData[MAX_NUM_DATA_OBJ];
  int                   xSize, ySize, i;
//  int                   Data_ySize;

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
//  Data_ySize = ySize - BORDER_BOTTOM;
  //
  // Create and configure GRAPH_SCALE objects
  //
  _hScaleH = GRAPH_SCALE_Create(BORDER_BOTTOM >> 1, GUI_TA_VCENTER, GRAPH_SCALE_CF_HORIZONTAL, TICK_DIST_H);
  _hScaleV = GRAPH_SCALE_Create(BORDER_LEFT   >> 1, GUI_TA_HCENTER, GRAPH_SCALE_CF_VERTICAL,   TICK_DIST_V);
  GRAPH_SCALE_SetPos(_hScaleH, ySize - SCALE_H_HEIGHT);
  GRAPH_SCALE_SetOff(_hScaleH, -5);
  // attach horizontal & vertical scale
  GRAPH_AttachScale(hGraph, _hScaleH);
  GRAPH_AttachScale(hGraph, _hScaleV);
  //
  // Show some graphs
  //
//  _DataAdjust = (Data_ySize * _aWave[0].DataVOff) >> GRAPH_DIV;
  _DataAdjust = WaveList[0].ScaleVOff;
  GRAPH_SetGridOffY (hGraph, 0);//(Data_ySize * _aWave[0].GridVOff) >> GRAPH_DIV
  GRAPH_SCALE_SetOff(_hScaleV, WaveList[0].ScaleVOff);//(((Data_ySize - BORDER_BOTTOM) * _aWave[0].ScaleVOff) >> GRAPH_DIV)
//  _ShowGraph(hGraph, hData, _aWave[0].NumWaves, _aWave[0].pfAddData);

  for(i = 0; i < WaveList[0].NumWaves; i ++)
    GRAPH_AttachData(hGraph, hData[i]);

  GUI_Exec();
  GRAPH_DetachScale(hGraph, _hScaleH);
  GRAPH_DetachScale(hGraph, _hScaleV);
  WM_ValidateWindow(hGraph);

  btn_evt_register_callback(&graph_btn_evt);

  do {
//    _aWave[0].pfAddData(hData[0], 0);
    for(i = 0; i < WaveList[0].NumWaves; i ++) {
      WaveList[0].pfAddData(hData[i], i);
    }
    GUI_Delay(15);
  } while(should_exit == 0);

  for(i = 0; i < WaveList[0].NumWaves; i ++)
    GRAPH_DetachData(hGraph, hData[i]);
//  i = 0;
//  while (_aWave[i].pfAddData != 0) {
////    GRAPH_AttachScale(hGraph, _hScaleH);
////    GRAPH_AttachScale(hGraph, _hScaleV);
//    _DataAdjust = (Data_ySize * _aWave[i].DataVOff) >> GRAPH_DIV;
//    GRAPH_SetGridOffY (hGraph, (Data_ySize * _aWave[i].GridVOff) >> GRAPH_DIV);
//    GRAPH_SCALE_SetOff(_hScaleV, (((Data_ySize - BORDER_BOTTOM) * _aWave[i].ScaleVOff) >> GRAPH_DIV));
//    _ShowGraph(hGraph, hData, _aWave[i].NumWaves, _aWave[i].pfAddData);
//    i++;
//  }
  btn_evt_unregister_callback(&graph_btn_evt);
  should_exit = 0;
  //
  // Clean up
  //
  GRAPH_DetachScale(hGraph, _hScaleH);
  GRAPH_DetachScale(hGraph, _hScaleV);
  GRAPH_SCALE_Delete(_hScaleH);
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
      ky_info("GRAPH", "print any log to test the CPU usage");
    }
  }
}
