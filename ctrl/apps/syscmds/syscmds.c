/*
 * syscmds.c
 *
 *  Created on: May 18, 2020
 *      Author: kychu
 */

#include "syscmds/syscmds.h"

#include "ctrl_task.h"
#include "att_est_q.h"
#include "rf_evt_proc.h"

#include "test/test.h"

extern int led_main(int argc, char *argv[]);

const shell_cmd_t sins_cmd = {
  .command = "sins",
  .help = "SINS",
  .hint = NULL,
  .func = &sins_main,
};

const shell_cmd_t ctrl_cmd = {
  .command = "ctrl",
  .help = "control",
  .hint = NULL,
  .func = &ctrl_main,
};

const shell_cmd_t rf_cmd = {
  .command = "rf",
  .help = "remote controller",
  .hint = NULL,
  .func = &remote_main,
};

const shell_cmd_t led_cmd = {
  .command = "led",
  .help = NULL,
  .hint = NULL,
  .func = &led_main,
};

void syscmds_register(void)
{
  shell_cmd_register(&sins_cmd);
  shell_cmd_register(&ctrl_cmd);
  shell_cmd_register(&rf_cmd);
  shell_cmd_register(&led_cmd);
  shell_cmd_register(&test_cmd);
}
