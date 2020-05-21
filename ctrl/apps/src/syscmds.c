/*
 * syscmds.c
 *
 *  Created on: May 18, 2020
 *      Author: kychu
 */

#include "syscmds.h"

#include "ctrl_task.h"
#include "att_est_q.h"
#include "rf_evt_proc.h"

extern int led_main(int argc, char *argv[]);

const console_cmd_t sins_cmd = {
  .command = "sins",
  .help = "SINS",
  .hint = NULL,
  .func = &sins_main,
};

const console_cmd_t ctrl_cmd = {
  .command = "ctrl",
  .help = "control",
  .hint = NULL,
  .func = &ctrl_main,
};

const console_cmd_t rf_cmd = {
  .command = "rf",
  .help = "remote controller",
  .hint = NULL,
  .func = &remote_main,
};

const console_cmd_t led_cmd = {
  .command = "led",
  .help = NULL,
  .hint = NULL,
  .func = &led_main,
};

void syscmds_register(void)
{
  console_cmd_register(&sins_cmd);
  console_cmd_register(&ctrl_cmd);
  console_cmd_register(&rf_cmd);
  console_cmd_register(&led_cmd);
}
