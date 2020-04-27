/*
 * disp.c
 *
 *  Created on: Apr 10, 2020
 *      Author: kychu
 */

#include "disp.h"

#define DISP_DRV_CHECK(x)                     {if((x) != status_ok) return status_error;}

static status_t disp_set_addr(disp_dev_t *hdisp, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);

status_t disp_init(disp_dev_t *hdisp)
{
  DISP_DRV_CHECK(hdisp->hw_init())

  DISP_DRV_CHECK(hdisp->wr_reg(0x01)) // software reset
  delay(10); //wait 10ms
  DISP_DRV_CHECK(hdisp->wr_reg(0x11)) // sleep out/power on(SLPOUT)
  delay(10);
  DISP_DRV_CHECK(hdisp->wr_reg(0x20)) // display inversion off
  delay(5);
  DISP_DRV_CHECK(hdisp->wr_reg(0x38)) // idle mode off
  delay(5);
  DISP_DRV_CHECK(hdisp->wr_reg(0xB1)) // normal mode
  DISP_DRV_CHECK(hdisp->wr_dat(0x05))
  DISP_DRV_CHECK(hdisp->wr_dat(0x3C))
  DISP_DRV_CHECK(hdisp->wr_dat(0x3C))

  DISP_DRV_CHECK(hdisp->wr_reg(0xB2)) //idle mode
  DISP_DRV_CHECK(hdisp->wr_dat(0x05))
  DISP_DRV_CHECK(hdisp->wr_dat(0x3C))
  DISP_DRV_CHECK(hdisp->wr_dat(0x3C))

  DISP_DRV_CHECK(hdisp->wr_reg(0xB3)) //partial mode
  DISP_DRV_CHECK(hdisp->wr_dat(0x05))
  DISP_DRV_CHECK(hdisp->wr_dat(0x3C))
  DISP_DRV_CHECK(hdisp->wr_dat(0x3C))
  DISP_DRV_CHECK(hdisp->wr_dat(0x05))
  DISP_DRV_CHECK(hdisp->wr_dat(0x3C))
  DISP_DRV_CHECK(hdisp->wr_dat(0x3C))

  DISP_DRV_CHECK(hdisp->wr_reg(0xB4)) //dot inversion
  DISP_DRV_CHECK(hdisp->wr_dat(0x03))

  DISP_DRV_CHECK(hdisp->wr_reg(0xC0)) //AVDD GVDD
  DISP_DRV_CHECK(hdisp->wr_dat(0xAB))
  DISP_DRV_CHECK(hdisp->wr_dat(0x0B))
  DISP_DRV_CHECK(hdisp->wr_dat(0x04))

  DISP_DRV_CHECK(hdisp->wr_reg(0xC1)) //VGH VGL
  DISP_DRV_CHECK(hdisp->wr_dat(0xC5)) //C0

  DISP_DRV_CHECK(hdisp->wr_reg(0xC2)) //Normal Mode
  DISP_DRV_CHECK(hdisp->wr_dat(0x0D))
  DISP_DRV_CHECK(hdisp->wr_dat(0x00))

  DISP_DRV_CHECK(hdisp->wr_reg(0xC3)) //idle
  DISP_DRV_CHECK(hdisp->wr_dat(0x8D))
  DISP_DRV_CHECK(hdisp->wr_dat(0x6A))

  DISP_DRV_CHECK(hdisp->wr_reg(0xC4)) //partial + full
  DISP_DRV_CHECK(hdisp->wr_dat(0x8D))
  DISP_DRV_CHECK(hdisp->wr_dat(0xEE))

  DISP_DRV_CHECK(hdisp->wr_reg(0xC5)) //VCOM
  DISP_DRV_CHECK(hdisp->wr_dat(0x0F))

  DISP_DRV_CHECK(hdisp->wr_reg(0xE0)) //positive gamma
  DISP_DRV_CHECK(hdisp->wr_dat(0x07))
  DISP_DRV_CHECK(hdisp->wr_dat(0x0E))
  DISP_DRV_CHECK(hdisp->wr_dat(0x08))
  DISP_DRV_CHECK(hdisp->wr_dat(0x07))
  DISP_DRV_CHECK(hdisp->wr_dat(0x10))
  DISP_DRV_CHECK(hdisp->wr_dat(0x07))
  DISP_DRV_CHECK(hdisp->wr_dat(0x02))
  DISP_DRV_CHECK(hdisp->wr_dat(0x07))
  DISP_DRV_CHECK(hdisp->wr_dat(0x09))
  DISP_DRV_CHECK(hdisp->wr_dat(0x0F))
  DISP_DRV_CHECK(hdisp->wr_dat(0x25))
  DISP_DRV_CHECK(hdisp->wr_dat(0x36))
  DISP_DRV_CHECK(hdisp->wr_dat(0x00))
  DISP_DRV_CHECK(hdisp->wr_dat(0x08))
  DISP_DRV_CHECK(hdisp->wr_dat(0x04))
  DISP_DRV_CHECK(hdisp->wr_dat(0x10))

  DISP_DRV_CHECK(hdisp->wr_reg(0xE1)) //negative gamma
  DISP_DRV_CHECK(hdisp->wr_dat(0x0A))
  DISP_DRV_CHECK(hdisp->wr_dat(0x0D))
  DISP_DRV_CHECK(hdisp->wr_dat(0x08))
  DISP_DRV_CHECK(hdisp->wr_dat(0x07))
  DISP_DRV_CHECK(hdisp->wr_dat(0x0F))
  DISP_DRV_CHECK(hdisp->wr_dat(0x07))
  DISP_DRV_CHECK(hdisp->wr_dat(0x02))
  DISP_DRV_CHECK(hdisp->wr_dat(0x07))
  DISP_DRV_CHECK(hdisp->wr_dat(0x09))
  DISP_DRV_CHECK(hdisp->wr_dat(0x0F))
  DISP_DRV_CHECK(hdisp->wr_dat(0x25))
  DISP_DRV_CHECK(hdisp->wr_dat(0x35))
  DISP_DRV_CHECK(hdisp->wr_dat(0x00))
  DISP_DRV_CHECK(hdisp->wr_dat(0x09))
  DISP_DRV_CHECK(hdisp->wr_dat(0x04))
  DISP_DRV_CHECK(hdisp->wr_dat(0x10))

  DISP_DRV_CHECK(hdisp->wr_reg(0xFC))
  DISP_DRV_CHECK(hdisp->wr_dat(0x80))

  DISP_DRV_CHECK(hdisp->wr_reg(0x3A))
  DISP_DRV_CHECK(hdisp->wr_dat(0x55)) // 16bit/pixel

  DISP_DRV_CHECK(hdisp->wr_reg(0x36))
  switch(hdisp->direction) {
  default:
  case DIRECT_TOP: DISP_DRV_CHECK(hdisp->wr_dat(0xC0 | 0x08)) break;    //→↓  MY=1;MX=1;MV=0;ML=0; RGB=0;MH=0;--;--;Update at H
  case DIRECT_BOTTOM: DISP_DRV_CHECK(hdisp->wr_dat(0x14 | 0x08)) break; //←↑  MY=0;MX=0;MV=0;ML=1; RGB=0;MH=1;--;--;Update at H
  case DIRECT_LEFT: DISP_DRV_CHECK(hdisp->wr_dat(0x64 | 0x08)) break;   //↑→  MY=0;MX=1;MV=1;ML=0; RGB=0;MH=1;--;--;Update at V
  case DIRECT_RIGHT: DISP_DRV_CHECK(hdisp->wr_dat(0xA4 | 0x08)) break;  //↓←  MY=1;MX=0;MV=1;ML=0; RGB=0;MH=1;--;--;Update at V
  }

  DISP_DRV_CHECK(hdisp->wr_reg(0x21)) //display inversion

  DISP_DRV_CHECK(hdisp->wr_reg(0x29)) //display on

//  DISP_DRV_CHECK(disp_set_addr(hdisp, 0, 0, hdisp->frame_width - 1, hdisp->frame_height - 1));
  return status_ok;
}

status_t disp_refresh(disp_dev_t *hdisp, uint8_t *buffer)
{
  DISP_DRV_CHECK(disp_set_addr(hdisp, 0, 0, hdisp->frame_width - 1, hdisp->frame_height - 1));
  return hdisp->wr_buf(buffer, hdisp->frame_width * hdisp->frame_height * 2);
}

status_t disp_display_on(disp_dev_t *hdisp)
{
  return hdisp->wr_reg(0x29);
}

status_t disp_display_off(disp_dev_t *hdisp)
{
  return hdisp->wr_reg(0x28);
}

static status_t disp_set_addr(disp_dev_t *hdisp, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
  switch(hdisp->direction) {
  default:
  case DIRECT_TOP:
  case DIRECT_BOTTOM:
    DISP_DRV_CHECK(hdisp->wr_reg(0x2A)) //set column address
    DISP_DRV_CHECK(hdisp->wr_dat((hdisp->ram_w_off + x1) >> 8))
    DISP_DRV_CHECK(hdisp->wr_dat((hdisp->ram_w_off + x1) & 0xFF)) // off = 26
    DISP_DRV_CHECK(hdisp->wr_dat((hdisp->ram_w_off + x2) >> 8))
    DISP_DRV_CHECK(hdisp->wr_dat((hdisp->ram_w_off + x2) & 0xFF)) // off = 26

    DISP_DRV_CHECK(hdisp->wr_reg(0x2B)) //set page address
    DISP_DRV_CHECK(hdisp->wr_dat((hdisp->ram_h_off + y1) >> 8))
    DISP_DRV_CHECK(hdisp->wr_dat((hdisp->ram_h_off + y1) & 0xFF)) // off = 1
    DISP_DRV_CHECK(hdisp->wr_dat((hdisp->ram_h_off + y2) >> 8))
    DISP_DRV_CHECK(hdisp->wr_dat((hdisp->ram_h_off + y2) & 0xFF)) // off = 1
  break;
  case DIRECT_LEFT:
  case DIRECT_RIGHT:
    DISP_DRV_CHECK(hdisp->wr_reg(0x2A)) //set column address
    DISP_DRV_CHECK(hdisp->wr_dat((hdisp->ram_h_off + x1) >> 8))
    DISP_DRV_CHECK(hdisp->wr_dat((hdisp->ram_h_off + x1) & 0xFF)) // off = 1
    DISP_DRV_CHECK(hdisp->wr_dat((hdisp->ram_h_off + x2) >> 8))
    DISP_DRV_CHECK(hdisp->wr_dat((hdisp->ram_h_off + x2) & 0xFF)) // off = 1

    DISP_DRV_CHECK(hdisp->wr_reg(0x2B)) //set page address
    DISP_DRV_CHECK(hdisp->wr_dat((hdisp->ram_w_off + y1) >> 8))
    DISP_DRV_CHECK(hdisp->wr_dat((hdisp->ram_w_off + y1) & 0xFF)) // off = 26
    DISP_DRV_CHECK(hdisp->wr_dat((hdisp->ram_w_off + y2) >> 8))
    DISP_DRV_CHECK(hdisp->wr_dat((hdisp->ram_w_off + y2) & 0xFF)) // off = 26
  break;
  }
  DISP_DRV_CHECK(hdisp->wr_reg(0x2C))

  return status_ok;
}
