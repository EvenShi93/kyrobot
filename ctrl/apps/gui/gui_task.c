/*
 * gui_task.c
 *
 *  Created on: Apr 10, 2020
 *      Author: kychu
 */

#include "apps.h"

#include "log.h"

#include "vscn.h"
#include "disp.h"

#include "WM.h"
#include "GUI.h"
#include "DIALOG.h"
#if CONFIG_STEMWIN_DEMO_ENABLE
#include "GUIDEMO.h"
#endif /* CONFIG_STEMWIN_DEMO_ENABLE */

#include "gui_demo.h"

#include "boot_logo.c"
#include "background.c"

static const char *TAG = "GUI";

vscn_handle_t *ips_ram;

void emwin_task(void const *argument);

static status_t ips_write_cmd(uint8_t cmd);
static status_t ips_write_dat(uint8_t dat);
static status_t ips_write_buf(uint8_t* buf, uint16_t size);

void gui_task(void const *argument)
{
  uint8_t *fb;
  disp_dev_t *ips_drv;

  ky_info(TAG, "GUI task started");

  fb = kmm_alloc(160 * 80 * 2);
  ips_drv = kmm_alloc(sizeof(disp_dev_t));
  ips_ram = kmm_alloc(sizeof(vscn_handle_t));
  if(fb == NULL || ips_ram == NULL || ips_drv == NULL) {
    ky_err(TAG, "no enough memory");
    goto exit;
  }

  ips_drv->frame_width = 160;
  ips_drv->frame_height = 80;
  ips_drv->direction = DIRECT_LEFT;
  ips_drv->ram_w_off = 26;
  ips_drv->ram_h_off = 1;

  ips_drv->hw_init = dispif_init;
  ips_drv->wr_reg = ips_write_cmd;
  ips_drv->wr_dat = ips_write_dat;
  ips_drv->wr_buf = ips_write_buf;

  ips_ram->ui_x = 160;
  ips_ram->ui_y = 80;
  ips_ram->ui_buf = fb;
  ips_ram->back_color = UI_COLOR_BLACK;
  ips_ram->text_color = 0xFFFF;

  output_port_clear(IO_DISP_RST);
  delay(100);
  output_port_set(IO_DISP_RST);
  delay(100);
  output_port_set(IO_DISP_BL);

  if(disp_init(ips_drv) != status_ok) {
    ky_err(TAG, "disp init failed");
    goto exit;
  }
  if(vscn_init(ips_ram) != status_ok) {
    ky_err(TAG, "gui init failed");
    goto exit;
  }
  vscn_clear(ips_ram);

  osThreadDef(EMWIN, emwin_task, osPriorityNormal, 0, 2048);
  if(osThreadCreate(osThread(EMWIN), NULL) == NULL) {
    ky_err(TAG, "emwin task create failed.");
  }

  for(;;) {
    delay(33);
    disp_refresh(ips_drv, ips_ram->ui_buf);
  }

exit:
  kmm_free(fb);
  kmm_free(ips_ram);
  kmm_free(ips_drv);
  vTaskDelete(NULL);
}

void emwin_task(void const *argument)
{
  __HAL_RCC_CRC_CLK_ENABLE(); /* Enable the CRC Module */

  /* STemwin Init */
  GUI_Init();

  /* Activate the use of memory device feature */
  WM_SetCreateFlags(WM_CF_MEMDEV);
  GUI_DrawBitmap(&bmboot_logo, 0, 15);
  delay(2000);
  for(;;) {
#if CONFIG_STEMWIN_DEMO_ENABLE
    /* Start Demo */
    GUIDEMO_Main();
#else
    gui_iconview_start();
    gui_graph_start();
    delay(500);
#endif /* CONFIG_STEMWIN_DEMO_ENABLE */
  }
}

static status_t ips_write_cmd(uint8_t cmd)
{
  uint8_t val = cmd;
  output_port_clear(IO_DISP_DC);
  return dispif_tx_bytes(&val, 1);
}

static status_t ips_write_dat(uint8_t dat)
{
  uint8_t val = dat;
  output_port_set(IO_DISP_DC);
  return dispif_tx_bytes(&val, 1);
}

static status_t ips_write_buf(uint8_t* buf, uint16_t size)
{
  output_port_set(IO_DISP_DC);
  return dispif_tx_bytes_dma(buf, size);
}
