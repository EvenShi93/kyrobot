/*
 * gui_demo.h
 *
 *  Created on: Apr 21, 2020
 *      Author: kychu
 */

#ifndef GUI_GUI_DEMO_H_
#define GUI_GUI_DEMO_H_

#include "GUI.h"

#include "WM.h"

typedef enum {
  menu_code_waves = 0,
  menu_code_system = 1,
  menu_code_nothing = 0xE,
  menu_code_poweroff = 0xF,
} main_menu_code_t;

main_menu_code_t gui_iconview_start(void);

void gui_graph_start(void);

int gui_setting_menu_start(void);

void gui_config_backlight_start(void);

void gui_ctrl_motor_start(void);
void gui_ctrl_steer_start(void);

#endif /* GUI_GUI_DEMO_H_ */
