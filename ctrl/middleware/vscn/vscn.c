/*
 * vscn.c
 *
 *  Created on: Apr 10, 2020
 *      Author: kychu
 */

#include "vscn.h"
#include <string.h>

#include "fonts.c"

status_t vscn_init(vscn_handle_t *hgui)
{
  if(hgui->ui_buf == NULL) return status_error;
  if(hgui->ui_x == 0 || hgui->ui_y == 0) return status_error;
  memset(hgui->ui_buf, 0, hgui->ui_x * hgui->ui_y * 2);
  return status_ok;
}

status_t vscn_clear(vscn_handle_t *hgui)
{
  for(int i = 0; i < hgui->ui_y; i ++) {
    for(int j = 0; j < hgui->ui_x; j ++) {
      ((uint16_t *)hgui->ui_buf)[i * hgui->ui_x + j] = hgui->back_color;
    }
  }
  return status_ok;
}

status_t vscn_draw_point(vscn_handle_t *hgui, uint16_t x, uint16_t y, uint16_t color)
{
  uint8_t *p;
  if((x < hgui->ui_x) && (y < hgui->ui_y)) {
//    ((uint16_t *)hgui->ui_buf)[y * hgui->ui_x + x] = color;
    p = hgui->ui_buf + (y * hgui->ui_x + x) * 2;
    *p = color >> 8;
    *(p + 1) = color;
    return status_ok;
  }
  return status_error;
}

status_t vscn_draw_v_line(vscn_handle_t *hgui, uint16_t x, uint16_t y, uint16_t l, uint16_t color)
{
  while(l --) {
    if((x < hgui->ui_x) && (y < hgui->ui_y)) {
//      ((uint16_t *)hgui->ui_buf)[y * hgui->ui_x + x] = color;
      vscn_draw_point(hgui, x, y, color);
    }
    y ++;
  }
  return status_ok;
}

//status_t gui_draw_char(gui_handle_t *hgui, uint16_t x, uint16_t y, const uint16_t *c)
//{
//  uint32_t index = 0, i = 0;
//	LCD_SetRamAddr(x, y + hgui->font->Width -1, Ypos, Ypos + LCD_Currentfonts->Height - 1);
//  for(index = 0; index < hgui->font->Height; index ++) {
//    for(i = 0; i < hgui->font->Width; i++) {
//      if(hgui->font->Width)
//      if((((c[index] & ((0x80 << ((hgui->font->Width / 12 ) << 3 ) ) >> i)) == 0x00) && (hgui->font->Width <= 12))||
//				(((c[index] & (0x1 << i)) == 0x00)&&(hgui->font->Width > 12 )))
//			{
//				WriteColor(BackColor);
//			}
//			else
//			{
//				WriteColor(TextColor);
//			}
//		}
//	}
//}
//
//status_t gui_clear_line(gui_handle_t *hgui, uint16_t Line)
//{
//	uint16_t refcolumn = 0;
//	/* Send the string character by character on lCD */
//	while ((refcolumn + hgui->font->Width) <= hgui->ui_x)
//	{
//		/* Display one character on LCD */
//		LCD_DisplayChar(Line, refcolumn, ' ');
//		/* Decrement the column position by 16 */
//		refcolumn += hgui->font->Width;
//	}
//}

status_t vscn_draw_circle(vscn_handle_t *hgui, uint8_t x, uint8_t y, uint8_t r, uint16_t color)
{
  int D;/* Decision Variable */
  uint16_t CurX;/* Current X Value */
  uint16_t CurY;/* Current Y Value */
  D = 3 - (r << 1);
  CurX = 0;
  CurY = r;
  while (CurX <= CurY) {
    vscn_draw_point(hgui, x + CurX, y + CurY, color);
    vscn_draw_point(hgui, x + CurX, y - CurY, color);
    vscn_draw_point(hgui, x - CurX, y + CurY, color);
    vscn_draw_point(hgui, x - CurX, y - CurY, color);
    vscn_draw_point(hgui, x + CurY, y + CurX, color);
    vscn_draw_point(hgui, x + CurY, y - CurX, color);
    vscn_draw_point(hgui, x - CurY, y + CurX, color);
    vscn_draw_point(hgui, x - CurY, y - CurX, color);
    if (D < 0) {
      D += (CurX << 2) + 6;
    } else {
      D += ((CurX - CurY) << 2) + 10;
      CurY--;
    }
    CurX++;
  }
  return status_ok;
}
