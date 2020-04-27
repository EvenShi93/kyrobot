/*
 * disp.h
 *
 *  Created on: Apr 10, 2020
 *      Author: kychu
 */

#ifndef BSP_INC_DISP_H_
#define BSP_INC_DISP_H_

#include "drivers.h"

typedef status_t (*dispif_hw_init)(void);
typedef status_t (*dispif_write_reg)(uint8_t);
typedef status_t (*dispif_write_dat)(uint8_t);
typedef status_t (*dispif_write_buf)(uint8_t*, uint16_t);

typedef enum {
  DIRECT_TOP = 0,
  DIRECT_BOTTOM = 1,
  DIRECT_LEFT = 2,
  DIRECT_RIGHT = 3
} disp_direction_t;

#define ROW_ADDR_ORDER         0x80
#define COL_ADDR_ORDER         0x40
#define ROW_COL_EXCHANGE       0x20
#define VERTICAL_REF_ORDER     0x10

typedef struct {
  uint16_t ram_w_off;
  uint16_t ram_h_off;
  uint16_t frame_width;
  uint16_t frame_height;
  disp_direction_t direction;
  dispif_hw_init hw_init;
  dispif_write_reg wr_reg;
  dispif_write_dat wr_dat;
  dispif_write_buf wr_buf;
} disp_dev_t;

status_t disp_init(disp_dev_t *hdisp);
status_t disp_refresh(disp_dev_t *hdisp, uint8_t *buffer);

status_t disp_display_on(disp_dev_t *hdisp);
status_t disp_display_off(disp_dev_t *hdisp);

#endif /* BSP_INC_DISP_H_ */
