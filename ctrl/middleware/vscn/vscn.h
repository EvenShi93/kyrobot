/*
 * vscn.h
 *
 *  Created on: Apr 10, 2020
 *      Author: kychu
 */

#ifndef VSCN_VSCN_H_
#define VSCN_VSCN_H_

#include "SysConfig.h"

#include "fonts.h"

typedef struct {
  uint16_t ui_x;
  uint16_t ui_y;
  uint8_t *ui_buf;
  sFONT *font;
  uint16_t text_color;
  uint16_t back_color;
} vscn_handle_t;

#define UI_COLOR_WHITE          0xFFFF
#define UI_COLOR_BLACK          0x0000
#define UI_COLOR_GREY           0xF7DE
#define UI_COLOR_BLUE           0x001F
#define UI_COLOR_BLUE2          0x051F
#define UI_COLOR_RED            0xF800
#define UI_COLOR_MAGENTA        0xF81F
#define UI_COLOR_GREEN          0x07E0
#define UI_COLOR_CYAN           0x7FFF
#define UI_COLOR_YELLOW         0xFFE0
#define UI_COLOR_GBLUE          0X07FF/* GREEN + BLUE */
#define UI_COLOR_BROWN          0XBC40/* BROWN */
#define UI_COLOR_BRRED          0XFC07/* BROWN + RED */
#define UI_COLOR_GRAY           0X8430/* BROWN */
#define UI_COLOR_DARKBLUE       0X01CF/* DARKBLUE */
#define UI_COLOR_LIGHTBLUE      0X7D7C/* LIGHTBLUE */
#define UI_COLOR_GRAYBLUE       0X5458/* GRAY + BLUE */

status_t vscn_init(vscn_handle_t *hscn);
status_t vscn_clear(vscn_handle_t *hscn);
status_t vscn_draw_point(vscn_handle_t *hscn, uint16_t x, uint16_t y, uint16_t color);
status_t vscn_draw_circle(vscn_handle_t *hscn, uint8_t x, uint8_t y, uint8_t r, uint16_t color);
status_t vscn_draw_v_line(vscn_handle_t *hscn, uint16_t x, uint16_t y, uint16_t l, uint16_t color);

#endif /* VSCN_VSCN_H_ */
