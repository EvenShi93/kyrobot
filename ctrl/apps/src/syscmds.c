/*
 * syscmds.c
 *
 *  Created on: May 18, 2020
 *      Author: kychu
 */

#include "syscmds.h"

#include "ctrl_task.h"
#include "att_est_q.h"

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

void syscmds_register(void)
{
  console_cmd_register(&sins_cmd);
  console_cmd_register(&ctrl_cmd);
}
